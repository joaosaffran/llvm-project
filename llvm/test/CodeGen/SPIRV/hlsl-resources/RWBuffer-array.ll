; RUN: llc -O0 -verify-machineinstrs -mtriple=spirv1.6-vulkan1.3-library %s -o - | FileCheck %s
; RUN: %if spirv-tools %{ llc -O0 -mtriple=spirv1.6-vulkan1.3-library %s -o - -filetype=obj | spirv-val %}

; CHECK-DAG: %[[int32:[0-9]+]] = OpTypeInt 32 0
; CHECK-DAG: %[[rwbuffer:[0-9]+]] = OpTypeImage %[[int32]] Buffer 2 0 0 2 R32i
; CHECK-DAG: OpTypeRuntimeArray %[[rwbuffer]]

; This IR was emmited from the following HLSL code:
; [[vk::binding(0)]]
; RWBuffer<int> Buf[] : register(u0);
; 
; [numthreads(4,2,1)]
; void main(uint GI : SV_GroupIndex) {
;     Buf[0][0] = 0;
; }


%"class.hlsl::RWBuffer" = type { target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) }

@Buf.str = private unnamed_addr constant [4 x i8] c"Buf\00", align 1

; Function Attrs: convergent noinline norecurse
define void @main() #0 {
entry:
  %this.addr.i8 = alloca ptr, align 8
  %other.addr.i9 = alloca ptr, align 8
  %this.addr.i6 = alloca ptr, align 8
  %this.addr.i4 = alloca ptr, align 8
  %other.addr.i = alloca ptr, align 8
  %this.addr.i2 = alloca ptr, align 8
  %this.addr.i = alloca ptr, align 8
  %Index.addr.i = alloca i32, align 4
  %result.ptr.i = alloca ptr, align 8
  %registerNo.addr.i = alloca i32, align 4
  %spaceNo.addr.i = alloca i32, align 4
  %range.addr.i = alloca i32, align 4
  %index.addr.i = alloca i32, align 4
  %name.addr.i = alloca ptr, align 8
  %tmp.i1 = alloca %"class.hlsl::RWBuffer", align 8
  %GI.addr.i = alloca i32, align 4
  %tmp.i = alloca %"class.hlsl::RWBuffer", align 8
  %0 = call token @llvm.experimental.convergence.entry()
  %1 = call i32 @llvm.spv.flattened.thread.id.in.group()
  store i32 %1, ptr %GI.addr.i, align 4
  store ptr %tmp.i, ptr %result.ptr.i, align 8
  store i32 0, ptr %registerNo.addr.i, align 4
  store i32 0, ptr %spaceNo.addr.i, align 4
  store i32 -1, ptr %range.addr.i, align 4
  store i32 0, ptr %index.addr.i, align 4
  store ptr @Buf.str, ptr %name.addr.i, align 8
  store ptr %tmp.i1, ptr %this.addr.i2, align 8
  %this1.i3 = load ptr, ptr %this.addr.i2, align 8
  store ptr %this1.i3, ptr %this.addr.i6, align 8
  %this1.i7 = load ptr, ptr %this.addr.i6, align 8
  store target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) poison, ptr %this1.i7, align 8
  %2 = load i32, ptr %registerNo.addr.i, align 4
  %3 = load i32, ptr %spaceNo.addr.i, align 4
  %4 = load i32, ptr %range.addr.i, align 4
  %5 = load i32, ptr %index.addr.i, align 4
  %6 = load ptr, ptr %name.addr.i, align 8
  %7 = call target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) @llvm.spv.resource.handlefrombinding.tspirv.SignedImage_i32_5_2_0_0_2_24t(i32 %3, i32 %2, i32 %4, i32 %5, ptr %6)
  store target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) %7, ptr %tmp.i1, align 8
  store ptr %tmp.i, ptr %this.addr.i4, align 8
  store ptr %tmp.i1, ptr %other.addr.i, align 8
  %this1.i5 = load ptr, ptr %this.addr.i4, align 8
  %8 = load ptr, ptr %other.addr.i, align 8
  store ptr %this1.i5, ptr %this.addr.i8, align 8
  store ptr %8, ptr %other.addr.i9, align 8
  %this1.i10 = load ptr, ptr %this.addr.i8, align 8
  %9 = load ptr, ptr %other.addr.i9, align 8
  %10 = load target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24), ptr %9, align 8
  store target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) %10, ptr %this1.i10, align 8
  store ptr %tmp.i, ptr %this.addr.i, align 8
  store i32 0, ptr %Index.addr.i, align 4
  %this1.i = load ptr, ptr %this.addr.i, align 8
  %11 = load target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24), ptr %this1.i, align 8
  %12 = load i32, ptr %Index.addr.i, align 4
  %13 = call noundef align 4 dereferenceable(4) ptr addrspace(11) @llvm.spv.resource.getpointer.p11.tspirv.SignedImage_i32_5_2_0_0_2_24t(target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) %11, i32 %12)
  store i32 0, ptr addrspace(11) %13, align 4
  ret void
}

; Function Attrs: nounwind willreturn memory(none)
declare i32 @llvm.spv.flattened.thread.id.in.group() #2

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24) @llvm.spv.resource.handlefrombinding.tspirv.SignedImage_i32_5_2_0_0_2_24t(i32, i32, i32, i32, ptr) #3

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(none)
declare ptr addrspace(11) @llvm.spv.resource.getpointer.p11.tspirv.SignedImage_i32_5_2_0_0_2_24t(target("spirv.SignedImage", i32, 5, 2, 0, 0, 2, 24), i32) #3

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: readwrite)
declare void @llvm.experimental.noalias.scope.decl(metadata) #4

attributes #0 = { convergent noinline norecurse "hlsl.numthreads"="4,2,1" "hlsl.shader"="compute" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #2 = { nounwind willreturn memory(none) }
attributes #3 = { nocallback nofree nosync nounwind willreturn memory(none) }
attributes #4 = { nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: readwrite) }
