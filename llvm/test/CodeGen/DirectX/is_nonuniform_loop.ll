; RUN: opt -S  -dxil-intrinsic-expansion -dxil-op-lower  -mtriple=dxil-pc-shadermodel6.3-library %s

; The goal of this test it to make sure compilation successfully,
; in bad case it should timeout.

@Out.str = private unnamed_addr constant [4 x i8] c"Out\00", align 1

; Function Attrs: nofree noinline norecurse nosync nounwind memory(inaccessiblemem: readwrite)
define void @main() local_unnamed_addr #0 {
entry:
  %cmp.i4.not = icmp eq i32 4, 0
  br i1 %cmp.i4.not, label %_Z4mainj.exit, label %for.body.i

for.body.i:                                       ; preds = %entry, %for.body.i
  %i.0.i5 = phi i32 [ %inc.i, %for.body.i ], [ 0, %entry ]
  %1 = tail call target("dx.RawBuffer", i32, 1, 0) @llvm.dx.resource.handlefrombinding(i32 0, i32 0, i32 4, i32 %i.0.i5, ptr nonnull @Out.str)
  %2 = tail call noundef i32 @llvm.dx.resource.updatecounter(target("dx.RawBuffer", i32, 1, 0) %1, i8 1)
  %inc.i = add nuw nsw i32 %i.0.i5, 1
  %exitcond.not = icmp eq i32 %inc.i, 4
  br i1 %exitcond.not, label %_Z4mainj.exit, label %for.body.i

_Z4mainj.exit:                                    ; preds = %for.body.i, %entry
  ret void
}


attributes #0 = { nofree noinline norecurse nosync nounwind memory(inaccessiblemem: readwrite) "frame-pointer"="all" "hlsl.numthreads"="4,1,1" "hlsl.shader"="compute" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
