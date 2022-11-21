; _____     ___ ____     ___ ____
;  ____|   |    ____|   |        | |____|
; |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
;-----------------------------------------------------------------------
; (c) 2020 h4570 Sandro Sobczyński <sandro.sobczynski@gmail.com>
; Licenced under Academic Free License version 2.0
; Review ps2sdk README & LICENSE files for further details.
;
;
;---------------------------------------------------------------
; draw_3D.vcl                                                   |
;---------------------------------------------------------------
; A VU1 microprogram to draw 3D object using XYZ2, RGBAQ and ST|
; This program uses double buffering (xtop)                    |
;                                                              |
; Many thanks to:                                              |
; - Dr Henry Fortuna                                           |
; - Jesper Svennevid, Daniel Collin                            |
; - Guilherme Lampert                                          |
;---------------------------------------------------------------

; The format of the data in the program (from xtop) should be:
; 0)    scale X, scale Y, scale Z, num_verts
; 1)    Color
; 2)    Transformation matrix row 0
; 3)    Transformation matrix row 1
; 4)    Transformation matrix row 2
; 5)    Transformation matrix row 3
; 6)    GIF Set tag
; 7)    Gif primitive tag
; 8)    Start of vertex data

.syntax new
.name VU1DrawLines
.vu
.init_vf_all
.init_vi_all

--enter
--endenter
    xtop    base
    fcset   0x0000

    ; Load the program parameters into registers
    lq.xyz      scale,          0(base)
    ilw.w       vertexCount,    0(base)
    lq          rgba,           1(base)

    lq          matrixRow0,     2(base)
    lq          matrixRow1,     3(base)
    lq          matrixRow2,     4(base)
    lq          matrixRow3,     5(base)

    lq          gifSetTag,      6(base)
    lq          gifPrimTag,     7(base)

    iaddiu      vertexData,     base,           8
    iadd        kickAddress,    vertexData,     vertexCount
    iadd        destAddress,    vertexData,     vertexCount

    sqi         gifSetTag,      (destAddress++)
    sqi         gifPrimTag,     (destAddress++)


    lq fakev, 0(vertexData)

    vertexLoop:
        lq          vertex, 0(vertexData)

        mul         acc,    matrixRow0,     vertex[x]
        madd        acc,    matrixRow1,     vertex[y]
        madd        acc,    matrixRow2,     vertex[z]
        madd        vertex, matrixRow3,     vertex[w]

        clipw.xyz   vertex, vertex
        fcand       vi01,   0x3FFFF
        iaddiu      iADC,   vi01,   0x7FFF

        isw.w       iADC,   1(destAddress)

        div         q,      vf00[w],        vertex[w]
        mul.xyz     vertex, vertex,         q
        mula.xyz    acc,    scale,          vf00[w]
        madd.xyz    vertex, vertex,         scale
        ftoi4       vertex, vertex

        sq          rgba,   0(destAddress)
        sq.xyz      fakev, 1(destAddress)

        iaddiu      vertexData,  vertexData,    1
        iaddiu      destAddress, destAddress,   2

        isubiu      vertexCount, vertexCount,   1
        ibgtz       vertexCount, vertexLoop

    --barrier
    xgkick kickAddress

--exit
--endexit