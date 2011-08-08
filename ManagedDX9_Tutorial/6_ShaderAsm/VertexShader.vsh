vs.1.1
;=========================================================================
; VertexShader.psh
; Coder: Thobias Jones
; Copyright (c) 2003 Thobias Jones. All rights reserved.
; Purpose: Simple assembly vertex shader
;=========================================================================

; input:
; c0 = WorldViewProjection transform matrix
; v0 = Vertex position
; v1 = Vertex diffuse color
; v2 = Vertex texture coords for texture map

; output:
; oPos = Transformed vertex position
; oD0  = Diffuse color
; oT0  = Vertex texture coords

dcl_position v0
dcl_color    v1
dcl_texcoord v2

m4x4 oPos, v0, c0              ; Transform the vertex into screen-space

; m4x4 is a macro and will expand to these instructions:
; dp4 oPos.x, v0, c[0]
; dp4 oPos.y, v0, c[1]
; dp4 oPos.z, v0, c[2]
; dp4 oPos.w, v0, c[3]

; HLSL knows about the Direct3D matrix format and will
; output this instead of m4x4. The advantage is that
; the program does not need to transpose the matrix
; before it is sent to the shader
; mul r0, v0.y, c1
; mad r0, c0, v0.x, r0
; mad r0, c2, v0.z, r0
; add oPos, r0, c3

mov oD0, v1                    ; Save the diffuse color
mov oT0.xy, v2                 ; Save texture coords
