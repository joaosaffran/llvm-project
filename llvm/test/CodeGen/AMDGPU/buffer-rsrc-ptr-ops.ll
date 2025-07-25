; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py UTC_ARGS: --version 2
; RUN: llc -global-isel=1 -mtriple=amdgcn -mcpu=gfx900 < %s | FileCheck --check-prefix=GISEL %s
; RUN: llc -global-isel=0 -mtriple=amdgcn -mcpu=gfx900 < %s | FileCheck --check-prefix=SDAG %s

define amdgpu_kernel void @buffer_ptr_vector_ops(ptr addrspace(1) %somewhere) {
; GISEL-LABEL: buffer_ptr_vector_ops:
; GISEL:       ; %bb.0: ; %main_body
; GISEL-NEXT:    s_load_dwordx2 s[8:9], s[4:5], 0x24
; GISEL-NEXT:    v_mov_b32_e32 v8, 0
; GISEL-NEXT:    s_waitcnt lgkmcnt(0)
; GISEL-NEXT:    s_load_dwordx8 s[0:7], s[8:9], 0x0
; GISEL-NEXT:    s_waitcnt lgkmcnt(0)
; GISEL-NEXT:    v_mov_b32_e32 v0, s0
; GISEL-NEXT:    v_mov_b32_e32 v4, s4
; GISEL-NEXT:    v_mov_b32_e32 v1, s1
; GISEL-NEXT:    v_mov_b32_e32 v2, s2
; GISEL-NEXT:    v_mov_b32_e32 v3, s3
; GISEL-NEXT:    v_mov_b32_e32 v5, s5
; GISEL-NEXT:    v_mov_b32_e32 v6, s6
; GISEL-NEXT:    v_mov_b32_e32 v7, s7
; GISEL-NEXT:    buffer_store_dwordx4 v[0:3], off, s[4:7], 0
; GISEL-NEXT:    global_store_dwordx4 v8, v[4:7], s[8:9] offset:32
; GISEL-NEXT:    global_store_dwordx4 v8, v[0:3], s[8:9] offset:48
; GISEL-NEXT:    s_endpgm
;
; SDAG-LABEL: buffer_ptr_vector_ops:
; SDAG:       ; %bb.0: ; %main_body
; SDAG-NEXT:    s_load_dwordx2 s[8:9], s[4:5], 0x24
; SDAG-NEXT:    v_mov_b32_e32 v8, 0
; SDAG-NEXT:    s_waitcnt lgkmcnt(0)
; SDAG-NEXT:    s_load_dwordx8 s[0:7], s[8:9], 0x0
; SDAG-NEXT:    s_waitcnt lgkmcnt(0)
; SDAG-NEXT:    v_mov_b32_e32 v0, s0
; SDAG-NEXT:    v_mov_b32_e32 v1, s1
; SDAG-NEXT:    v_mov_b32_e32 v2, s2
; SDAG-NEXT:    v_mov_b32_e32 v3, s3
; SDAG-NEXT:    v_mov_b32_e32 v4, s4
; SDAG-NEXT:    v_mov_b32_e32 v5, s5
; SDAG-NEXT:    v_mov_b32_e32 v6, s6
; SDAG-NEXT:    v_mov_b32_e32 v7, s7
; SDAG-NEXT:    buffer_store_dwordx4 v[0:3], off, s[4:7], 0
; SDAG-NEXT:    global_store_dwordx4 v8, v[0:3], s[8:9] offset:48
; SDAG-NEXT:    global_store_dwordx4 v8, v[4:7], s[8:9] offset:32
; SDAG-NEXT:    s_endpgm
main_body:
  %buffers = load <2 x ptr addrspace(8)>, ptr addrspace(1) %somewhere
  %buf1 = extractelement <2 x ptr addrspace(8)> %buffers, i32 0
  %buf2 = extractelement <2 x ptr addrspace(8)> %buffers, i32 1
  %buf1.int = ptrtoint ptr addrspace(8) %buf1 to i128
  %buf1.vec = bitcast i128 %buf1.int to <4 x i32>
  call void @llvm.amdgcn.raw.ptr.buffer.store.v4i32(<4 x i32> %buf1.vec, ptr addrspace(8) %buf2, i32 0, i32 0, i32 0)
  %shuffled = shufflevector <2 x ptr addrspace(8)> %buffers, <2 x ptr addrspace(8)> poison, <2 x i32> <i32 1, i32 0>
  %somewhere.next = getelementptr <2 x ptr addrspace(8)>, ptr addrspace(1) %somewhere, i64 1
  store <2 x ptr addrspace(8)> %shuffled, ptr addrspace(1) %somewhere.next
  ret void
}

%fat_buffer_struct = type {ptr addrspace(8), i32}

define amdgpu_kernel void @buffer_structs(%fat_buffer_struct %arg, ptr addrspace(1) %dest) {
; GISEL-LABEL: buffer_structs:
; GISEL:       ; %bb.0: ; %main_body
; GISEL-NEXT:    s_load_dword s6, s[4:5], 0x34
; GISEL-NEXT:    s_load_dwordx4 s[0:3], s[4:5], 0x24
; GISEL-NEXT:    s_load_dwordx2 s[8:9], s[4:5], 0x44
; GISEL-NEXT:    v_mov_b32_e32 v5, 0
; GISEL-NEXT:    s_waitcnt lgkmcnt(0)
; GISEL-NEXT:    s_ashr_i32 s7, s6, 31
; GISEL-NEXT:    s_lshl_b64 s[4:5], s[6:7], 5
; GISEL-NEXT:    s_add_u32 s4, s8, s4
; GISEL-NEXT:    v_mov_b32_e32 v0, s0
; GISEL-NEXT:    v_mov_b32_e32 v4, s6
; GISEL-NEXT:    s_addc_u32 s5, s9, s5
; GISEL-NEXT:    v_mov_b32_e32 v1, s1
; GISEL-NEXT:    v_mov_b32_e32 v2, s2
; GISEL-NEXT:    v_mov_b32_e32 v3, s3
; GISEL-NEXT:    buffer_store_dword v4, v4, s[0:3], 0 offen
; GISEL-NEXT:    global_store_dwordx4 v5, v[0:3], s[4:5]
; GISEL-NEXT:    global_store_dword v5, v4, s[4:5] offset:16
; GISEL-NEXT:    s_endpgm
;
; SDAG-LABEL: buffer_structs:
; SDAG:       ; %bb.0: ; %main_body
; SDAG-NEXT:    s_load_dword s6, s[4:5], 0x34
; SDAG-NEXT:    s_load_dwordx4 s[0:3], s[4:5], 0x24
; SDAG-NEXT:    s_load_dwordx2 s[8:9], s[4:5], 0x44
; SDAG-NEXT:    v_mov_b32_e32 v4, 0
; SDAG-NEXT:    s_waitcnt lgkmcnt(0)
; SDAG-NEXT:    s_ashr_i32 s7, s6, 31
; SDAG-NEXT:    s_lshl_b64 s[4:5], s[6:7], 5
; SDAG-NEXT:    s_add_u32 s4, s8, s4
; SDAG-NEXT:    v_mov_b32_e32 v0, s6
; SDAG-NEXT:    s_addc_u32 s5, s9, s5
; SDAG-NEXT:    buffer_store_dword v0, v0, s[0:3], 0 offen
; SDAG-NEXT:    global_store_dword v4, v0, s[4:5] offset:16
; SDAG-NEXT:    v_mov_b32_e32 v0, s0
; SDAG-NEXT:    v_mov_b32_e32 v1, s1
; SDAG-NEXT:    v_mov_b32_e32 v2, s2
; SDAG-NEXT:    v_mov_b32_e32 v3, s3
; SDAG-NEXT:    global_store_dwordx4 v4, v[0:3], s[4:5]
; SDAG-NEXT:    s_endpgm
main_body:
  %buffer = extractvalue %fat_buffer_struct %arg, 0
  %offset = extractvalue %fat_buffer_struct %arg, 1
  call void @llvm.amdgcn.raw.ptr.buffer.store.i32(i32 %offset, ptr addrspace(8) %buffer, i32 %offset, i32 0, i32 0)
  ; Confirm the alignment of this struct is 32 bytes
  %dest.next = getelementptr %fat_buffer_struct, ptr addrspace(1) %dest, i32 %offset
  store %fat_buffer_struct %arg, ptr addrspace(1) %dest.next
  ret void
}

declare void @llvm.amdgcn.raw.ptr.buffer.store.i32(i32, ptr addrspace(8), i32, i32, i32 immarg)
declare void @llvm.amdgcn.raw.ptr.buffer.store.v4i32(<4 x i32>, ptr addrspace(8), i32, i32, i32 immarg)
