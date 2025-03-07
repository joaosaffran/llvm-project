; RUN: opt %s -dxil-embed -dxil-globals -S -o - | FileCheck %s
; RUN: opt -passes='print<dxil-root-signature>' %s -S -o - 2>&1 | FileCheck %s --check-prefix=DXC

target triple = "dxil-unknown-shadermodel6.0-compute"

; CHECK: @dx.rts0 = private constant [96 x i8]  c"{{.*}}", section "RTS0", align 4

define void @main() #0 {
entry:
  ret void
}
attributes #0 = { "hlsl.numthreads"="1,1,1" "hlsl.shader"="compute" }


!dx.rootsignatures = !{!2} ; list of function/root signature pairs
!2 = !{ ptr @main, !3 } ; function, root signature
!3 = !{ !4, !5, !6 } ; list of root signature elements
!4 = !{ !"RootCBV", i32 0, i32 1, i32 0, i32 1 }
!5 = !{ !"RootSRV", i32 1, i32 3, i32 4, i32 2 }
!6 = !{ !"RootUAV", i32 2, i32 5, i32 6, i32 3 }

; DXC: Definition for 'main':
; DXC-NEXT:  Flags: 0x000000:
; DXC-NEXT:  Version: 2:
; DXC-NEXT:  NumParameters: 3:
; DXC-NEXT:  RootParametersOffset: 24:
; DXC-NEXT:  NumStaticSamplers: 0:
; DXC-NEXT:  StaticSamplersOffset: 264:
; DXC-NEXT: - Parameters: 
; DXC-NEXT:   Type: 2 
; DXC-NEXT:   ShaderVisibility: 0 
; DXC-NEXT:    - Descriptor: 
; DXC-NEXT:     ShaderSpace: 0 
; DXC-NEXT:     ShaderRegistry: 1 
; DXC-NEXT:     DescriptorFlag: 1 
; DXC-NEXT:   Type: 3 
; DXC-NEXT:   ShaderVisibility: 1 
; DXC-NEXT:    - Descriptor: 
; DXC-NEXT:     ShaderSpace: 4 
; DXC-NEXT:     ShaderRegistry: 3 
; DXC-NEXT:     DescriptorFlag: 2 
; DXC-NEXT:   Type: 4 
; DXC-NEXT:   ShaderVisibility: 2 
; DXC-NEXT:    - Descriptor: 
; DXC-NEXT:     ShaderSpace: 6 
; DXC-NEXT:     ShaderRegistry: 5 
; DXC-NEXT:     DescriptorFlag: 3 
