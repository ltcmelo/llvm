; RUN: llc < %s -march=r600 -mcpu=verde -verify-machineinstrs | FileCheck --check-prefix=SI --check-prefix=FUNC %s
; RUN: llc < %s -march=r600 -mcpu=bonaire -verify-machineinstrs | FileCheck --check-prefix=CI --check-prefix=FUNC %s

; On Southern Islands GPUs the local address space(3) uses 32-bit pointers and
; the global address space(1) uses 64-bit pointers.  These tests check to make sure
; the correct pointer size is used for the local address space.

; The e{{32|64}} suffix on the instructions refers to the encoding size and not
; the size of the operands.  The operand size is denoted in the instruction name.
; Instructions with B32, U32, and I32 in their name take 32-bit operands, while
; instructions with B64, U64, and I64 take 64-bit operands.

; FUNC-LABEL: @local_address_load
; CHECK: V_MOV_B32_e{{32|64}} [[PTR:v[0-9]]]
; CHECK: DS_READ_B32 v{{[0-9]+}}, [[PTR]]
define void @local_address_load(i32 addrspace(1)* %out, i32 addrspace(3)* %in) {
entry:
  %0 = load i32 addrspace(3)* %in
  store i32 %0, i32 addrspace(1)* %out
  ret void
}

; FUNC-LABEL: @local_address_gep
; CHECK: S_ADD_I32 [[SPTR:s[0-9]]]
; CHECK: V_MOV_B32_e32 [[VPTR:v[0-9]+]], [[SPTR]]
; CHECK: DS_READ_B32 [[VPTR]]
define void @local_address_gep(i32 addrspace(1)* %out, i32 addrspace(3)* %in, i32 %offset) {
entry:
  %0 = getelementptr i32 addrspace(3)* %in, i32 %offset
  %1 = load i32 addrspace(3)* %0
  store i32 %1, i32 addrspace(1)* %out
  ret void
}

; FUNC-LABEL: @local_address_gep_const_offset
; CHECK: V_MOV_B32_e32 [[VPTR:v[0-9]+]], s{{[0-9]+}}
; CHECK: DS_READ_B32 v{{[0-9]+}}, [[VPTR]], 0x4,
define void @local_address_gep_const_offset(i32 addrspace(1)* %out, i32 addrspace(3)* %in) {
entry:
  %0 = getelementptr i32 addrspace(3)* %in, i32 1
  %1 = load i32 addrspace(3)* %0
  store i32 %1, i32 addrspace(1)* %out
  ret void
}

; Offset too large, can't fold into 16-bit immediate offset.
; FUNC-LABEL: @local_address_gep_large_const_offset
; CHECK: S_ADD_I32 [[SPTR:s[0-9]]], s{{[0-9]+}}, 0x10004
; CHECK: V_MOV_B32_e32 [[VPTR:v[0-9]+]], [[SPTR]]
; CHECK: DS_READ_B32 [[VPTR]]
define void @local_address_gep_large_const_offset(i32 addrspace(1)* %out, i32 addrspace(3)* %in) {
entry:
  %0 = getelementptr i32 addrspace(3)* %in, i32 16385
  %1 = load i32 addrspace(3)* %0
  store i32 %1, i32 addrspace(1)* %out
  ret void
}

; FUNC-LABEL: @null_32bit_lds_ptr:
; CHECK: V_CMP_NE_I32
; CHECK-NOT: V_CMP_NE_I32
; CHECK: V_CNDMASK_B32
define void @null_32bit_lds_ptr(i32 addrspace(1)* %out, i32 addrspace(3)* %lds) nounwind {
  %cmp = icmp ne i32 addrspace(3)* %lds, null
  %x = select i1 %cmp, i32 123, i32 456
  store i32 %x, i32 addrspace(1)* %out
  ret void
}

; FUNC-LABEL: @mul_32bit_ptr:
; CHECK: V_MUL_LO_I32
; CHECK-NEXT: V_ADD_I32_e32
; CHECK-NEXT: DS_READ_B32
define void @mul_32bit_ptr(float addrspace(1)* %out, [3 x float] addrspace(3)* %lds, i32 %tid) {
  %ptr = getelementptr [3 x float] addrspace(3)* %lds, i32 %tid, i32 0
  %val = load float addrspace(3)* %ptr
  store float %val, float addrspace(1)* %out
  ret void
}

@g_lds = addrspace(3) global float zeroinitializer, align 4

; FUNC-LABEL: @infer_ptr_alignment_global_offset:
; CHECK: V_MOV_B32_e32 [[REG:v[0-9]+]], 0
; CHECK: DS_READ_B32 v{{[0-9]+}}, [[REG]]
define void @infer_ptr_alignment_global_offset(float addrspace(1)* %out, i32 %tid) {
  %val = load float addrspace(3)* @g_lds
  store float %val, float addrspace(1)* %out
  ret void
}


@ptr = addrspace(3) global i32 addrspace(3)* null
@dst = addrspace(3) global [16384 x i32] zeroinitializer

; FUNC-LABEL: @global_ptr:
; CHECK: DS_WRITE_B32
define void @global_ptr() nounwind {
  store i32 addrspace(3)* getelementptr ([16384 x i32] addrspace(3)* @dst, i32 0, i32 16), i32 addrspace(3)* addrspace(3)* @ptr
  ret void
}

; FUNC-LABEL: @local_address_store
; CHECK: DS_WRITE_B32
define void @local_address_store(i32 addrspace(3)* %out, i32 %val) {
  store i32 %val, i32 addrspace(3)* %out
  ret void
}

; FUNC-LABEL: @local_address_gep_store
; CHECK: S_ADD_I32 [[SADDR:s[0-9]+]],
; CHECK: V_MOV_B32_e32 [[ADDR:v[0-9]+]], [[SADDR]]
; CHECK: DS_WRITE_B32 [[ADDR]], v{{[0-9]+}},
define void @local_address_gep_store(i32 addrspace(3)* %out, i32, i32 %val, i32 %offset) {
  %gep = getelementptr i32 addrspace(3)* %out, i32 %offset
  store i32 %val, i32 addrspace(3)* %gep, align 4
  ret void
}

; FUNC-LABEL: @local_address_gep_const_offset_store
; CHECK: V_MOV_B32_e32 [[VPTR:v[0-9]+]], s{{[0-9]+}}
; CHECK: V_MOV_B32_e32 [[VAL:v[0-9]+]], s{{[0-9]+}}
; CHECK: DS_WRITE_B32 [[VPTR]], [[VAL]], 0x4
define void @local_address_gep_const_offset_store(i32 addrspace(3)* %out, i32 %val) {
  %gep = getelementptr i32 addrspace(3)* %out, i32 1
  store i32 %val, i32 addrspace(3)* %gep, align 4
  ret void
}

; Offset too large, can't fold into 16-bit immediate offset.
; FUNC-LABEL: @local_address_gep_large_const_offset_store
; CHECK: S_ADD_I32 [[SPTR:s[0-9]]], s{{[0-9]+}}, 0x10004
; CHECK: V_MOV_B32_e32 [[VPTR:v[0-9]+]], [[SPTR]]
; CHECK: DS_WRITE_B32 [[VPTR]], v{{[0-9]+}}, 0
define void @local_address_gep_large_const_offset_store(i32 addrspace(3)* %out, i32 %val) {
  %gep = getelementptr i32 addrspace(3)* %out, i32 16385
  store i32 %val, i32 addrspace(3)* %gep, align 4
  ret void
}
