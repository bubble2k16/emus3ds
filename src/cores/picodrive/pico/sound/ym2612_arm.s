/*
 * PicoDrive
 * (C) notaz, 2006
 *
 * This work is licensed under the terms of MAME license.
 * See COPYING file in the top-level directory.
 */

@ this is a rewrite of MAME's ym2612 code, in particular this is only the main sample-generatin loop.
@ it does not seem to give much performance increase (if any at all), so don't use it if it causes trouble.
@ - notaz, 2006

@ vim:filetype=armasm

.equiv SLOT1, 0
.equiv SLOT2, 2
.equiv SLOT3, 1
.equiv SLOT4, 3
.equiv SLOT_STRUCT_SIZE, 0x30

.equiv TL_TAB_LEN, 0x1A00

.equiv EG_ATT, 4
.equiv EG_DEC, 3
.equiv EG_SUS, 2
.equiv EG_REL, 1
.equiv EG_OFF, 0

.equiv EG_SH,			  16             @ 16.16 fixed point (envelope generator timing)
.equiv EG_TIMER_OVERFLOW, (3*(1<<EG_SH)) @ envelope generator timer overflows every 3 samples (on real chip)
.equiv LFO_SH,            25  /*  7.25 fixed point (LFO calculations)       */

.equiv ENV_QUIET,		  (2*13*256/8)

.text
.align 2

@ r5=slot, r1=eg_cnt, trashes: r0,r2,r3
@ writes output to routp, but only if vol_out changes
.macro update_eg_phase_slot slot
    ldrb    r2, [r5,#0x17]       @ state
    add     r3, r5, #0x1c
    tst     r2, r2
    beq     0f                   @ EG_OFF

    ldr     r2, [r3, r2, lsl #2] @ pack
    mov     r3, #1
    mov     r0, r2, lsr #24      @ shift
    mov     r3, r3, lsl r0
    sub     r3, r3, #1

    tst     r1, r3
    bne     0f                   @ no volume change

    mov     r3, r1, lsr r0
    and     r3, r3, #7
    add     r3, r3, r3, lsl #1
    mov     r3, r2, lsr r3
    and     r3, r3, #7           @ eg_inc_val shift, may be 0
    ldrb    r2, [r5,#0x17]       @ state
    ldrh    r0, [r5,#0x1a]       @ volume, unsigned (0-1023)

    cmp     r2, #4               @ EG_ATT
    beq     4f
    cmp     r2, #2
    mov     r2, #1
    mov     r2, r2, lsl r3
    mov     r2, r2, lsr #1       @ eg_inc_val
    add     r0, r0, r2
    blt     1f                   @ EG_REL
    beq     2f                   @ EG_SUS

3:  @ EG_DEC
    ldr     r2, [r5,#0x1c]       @ sl (can be 16bit?)
    mov     r3, #EG_SUS
    cmp     r0, r2               @ if ( volume >= (INT32) SLOT->sl )
    strgeb  r3, [r5,#0x17]       @ state
    b       10f

4:  @ EG_ATT
    subs    r3, r3, #1           @ eg_inc_val_shift - 1
    mov     r2, #0
    mvnpl   r2, r0
    mov     r2, r2, lsl r3
    add     r0, r0, r2, asr #4
    cmp     r0, #0               @ if (volume <= MIN_ATT_INDEX)
    movle   r3, #EG_DEC
    strleb  r3, [r5,#0x17]       @ state
    movle   r0, #0
    b       10f

2:  @ EG_SUS
    mov     r2, #1024
    sub     r2, r2, #1           @ r2 = MAX_ATT_INDEX
    cmp     r0, r2               @ if ( volume >= MAX_ATT_INDEX )
    movge   r0, r2
    b       10f

1:  @ EG_REL
    mov     r2, #1024
    sub     r2, r2, #1           @ r2 = MAX_ATT_INDEX
    cmp     r0, r2               @ if ( volume >= MAX_ATT_INDEX )
    movge   r0, r2
    movge   r3, #EG_OFF
    strgeb  r3, [r5,#0x17]       @ state

10: @ finish
    ldrh    r3, [r5,#0x18]       @ tl
    strh    r0, [r5,#0x1a]       @ volume
.if     \slot == SLOT1
    mov     r6, r6, lsr #16
    add     r0, r0, r3
    orr     r6, r0, r6, lsl #16
.elseif \slot == SLOT2
    mov     r6, r6, lsl #16
    add     r0, r0, r3
    mov     r0, r0, lsl #16
    orr     r6, r0, r6, lsr #16
.elseif \slot == SLOT3
    mov     r7, r7, lsr #16
    add     r0, r0, r3
    orr     r7, r0, r7, lsl #16
.elseif \slot == SLOT4
    mov     r7, r7, lsl #16
    add     r0, r0, r3
    mov     r0, r0, lsl #16
    orr     r7, r0, r7, lsr #16
.endif

0: @ EG_OFF
.endm


@ r12=lfo_ampm[31:16], r1=lfo_cnt_old, r2=lfo_cnt, r3=scratch
.macro advance_lfo_m
    mov     r2, r2, lsr #LFO_SH
    cmp     r2, r1, lsr #LFO_SH
    beq     0f
    and     r3, r2, #0x3f
    cmp     r2, #0x40
    rsbge   r3, r3, #0x3f
    bic     r12,r12, #0xff000000          @ lfo_ampm &= 0xff
    orr     r12,r12, r3, lsl #1+24

    mov     r2, r2, lsr #2
    cmp     r2, r1, lsr #LFO_SH+2
    bicne   r12,r12, #0xff0000
    orrne   r12,r12, r2, lsl #16

0:
.endm


@ result goes to r1, trashes r2
.macro make_eg_out slot
    tst     r12, #8
    tstne   r12, #(1<<(\slot+8))
.if     \slot == SLOT1
    mov     r1, r6, lsl #16
    mov     r1, r1, lsr #16
.elseif \slot == SLOT2
    mov     r1, r6, lsr #16
.elseif \slot == SLOT3
    mov     r1, r7, lsl #16
    mov     r1, r1, lsr #16
.elseif \slot == SLOT4
    mov     r1, r7, lsr #16
.endif
    andne   r2, r12, #0xc0
    movne   r2, r2,  lsr #6
    addne   r2, r2,  #24
    addne   r1, r1,  r12, lsr r2
    bic     r1, r1,  #1
.endm


@ \r=sin/result, r1=env, r3=ym_tl_tab
.macro lookup_tl r
    tst     \r, #0x100
    eorne   \r, \r, #0xff   @ if (sin & 0x100) sin = 0xff - (sin&0xff);
    tst     \r, #0x200
    and     \r, \r, #0xff
    orr     \r, \r, r1, lsl #7
    mov     \r, \r, lsl #1
    ldrh    \r, [r3, \r]    @ 2ci if ne
    rsbne   \r, \r, #0
.endm


@ lr=context, r12=pack (stereo, lastchan, disabled, lfo_enabled | pan_r, pan_l, ams[2] | AMmasks[4] | FB[4] | lfo_ampm[16])
@ r0-r2=scratch, r3=sin_tab, r5=scratch, r6-r7=vol_out[4], r10=op1_out
.macro upd_algo0_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r2, [lr, #0x18]
    ldr     r0, [lr, #0x38] @ mem (signed)
    mov     r2, r2, lsr #16
    add     r0, r2, r0, lsr #1
    lookup_tl r0                  @ r0=c2

0:

    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r0, r0, lsr #1
    add     r0, r0, r2, lsr #16
    lookup_tl r0                  @ r0=output smp

1:
    @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    movcs   r2, #0
    bcs     2f
    ldr     r2, [lr, #0x14]       @ 1ci
    mov     r5, r10, lsr #17
    add     r2, r5, r2, lsr #16
    lookup_tl r2                  @ r2=mem

2:
    str     r2, [lr, #0x38] @ mem
.endm


.macro upd_algo1_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r2, [lr, #0x18]
    ldr     r0, [lr, #0x38] @ mem (signed)
    mov     r2, r2, lsr #16
    add     r0, r2, r0, lsr #1
    lookup_tl r0                 @ r0=c2

0:
    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r0, r0, lsr #1
    add     r0, r0, r2, lsr #16
    lookup_tl r0                 @ r0=output smp

1:
    @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    movcs   r2, #0
    bcs     2f
    ldr     r2, [lr, #0x14]      @ 1ci
    mov     r2, r2, lsr #16
    lookup_tl r2                 @ r2=mem

2:
    add     r2, r2, r10, asr #16
    str     r2, [lr, #0x38]
.endm


.macro upd_algo2_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r2, [lr, #0x18]
    ldr     r0, [lr, #0x38] @ mem (signed)
    mov     r2, r2, lsr #16
    add     r0, r2, r0, lsr #1
    lookup_tl r0                 @ r0=c2

0:
    add     r0, r0, r10, asr #16

    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r0, r0, lsr #1
    add     r0, r0, r2, lsr #16
    lookup_tl r0                 @ r0=output smp

1:
    @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    movcs   r2, #0
    bcs     2f
    ldr     r2, [lr, #0x14]
    mov     r2, r2, lsr #16      @ 1ci
    lookup_tl r2                 @ r2=mem

2:
    str     r2, [lr, #0x38] @ mem
.endm


.macro upd_algo3_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    ldr     r2, [lr, #0x38] @ mem (for future)
    mov     r0, #0
    bcs     0f
    ldr     r0, [lr, #0x18]      @ phase3
    mov     r0, r0, lsr #16
    lookup_tl r0                 @ r0=c2

0:
    add     r0, r0, r2

    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r0, r0, lsr #1
    add     r0, r0, r2, lsr #16
    lookup_tl r0                 @ r0=output smp

1:
    @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    movcs   r2, #0
    bcs     2f
    ldr     r2, [lr, #0x14]      @ phase2
    mov     r5, r10, lsr #17
    add     r2, r5, r2, lsr #16
    lookup_tl r2                 @ r2=mem

2:
    str     r2, [lr, #0x38]      @ mem
.endm


.macro upd_algo4_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r0, [lr, #0x18]
    mov     r0, r0, lsr #16      @ 1ci
    lookup_tl r0                 @ r0=c2

0:
    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r0, r0, lsr #1
    add     r0, r0, r2, lsr #16
    lookup_tl r0                 @ r0=output smp

1:
    @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    bcs     2f
    ldr     r2, [lr, #0x14]
    mov     r5, r10, lsr #17
    add     r2, r5, r2, lsr #16
    lookup_tl r2
    add     r0, r0, r2            @ add to smp

2:
.endm


.macro upd_algo5_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r2, [lr, #0x18]
    ldr     r0, [lr, #0x38] @ mem (signed)
    mov     r2, r2, lsr #16
    add     r0, r2, r0, lsr #1
    lookup_tl r0                 @ r0=output smp

0:
    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r5, r10, lsr #17
    add     r2, r5, r2, lsr #16
    lookup_tl r2
    add     r0, r0, r2           @ add to smp

1:  @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    bcs     2f
    ldr     r2, [lr, #0x14]
    mov     r5, r10, lsr #17
    add     r2, r5, r2, lsr #16
    lookup_tl r2
    add     r0, r0, r2           @ add to smp

2:
    mov     r1, r10, asr #16
    str     r1, [lr, #0x38] @ mem
.endm


.macro upd_algo6_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r0, [lr, #0x18]
    mov     r0, r0, lsr #16      @ 1ci
    lookup_tl r0                 @ r0=output smp

0:
    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r2, r2, lsr #16      @ 1ci
    lookup_tl r2
    add     r0, r0, r2           @ add to smp

1:  @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    bcs     2f
    ldr     r2, [lr, #0x14]
    mov     r5, r10, lsr #17
    add     r2, r5, r2, lsr #16
    lookup_tl r2
    add     r0, r0, r2           @ add to smp

2:
.endm


.macro upd_algo7_m

    @ SLOT3
    make_eg_out SLOT3
    cmp     r1, #ENV_QUIET
    movcs   r0, #0
    bcs     0f
    ldr     r0, [lr, #0x18]
    mov     r0, r0, lsr #16      @ 1ci
    lookup_tl r0                 @ r0=output smp

0:
    add     r0, r0, r10, asr #16

    @ SLOT4
    make_eg_out SLOT4
    cmp     r1, #ENV_QUIET
    bcs     1f
    ldr     r2, [lr, #0x1c]
    mov     r2, r2, lsr #16      @ 1ci
    lookup_tl r2
    add     r0, r0, r2           @ add to smp

1:  @ SLOT2
    make_eg_out SLOT2
    cmp     r1, #ENV_QUIET
    bcs     2f
    ldr     r2, [lr, #0x14]
    mov     r2, r2, lsr #16      @ 1ci
    lookup_tl r2
    add     r0, r0, r2           @ add to smp

2:
.endm


.macro upd_slot1_m

    make_eg_out SLOT1
    cmp     r1, #ENV_QUIET
    movcs   r10, r10, lsl #16     @ ct->op1_out <<= 16; // op1_out0 = op1_out1; op1_out1 = 0;
    bcs     0f
    ands    r2, r12, #0xf000
    moveq   r0, #0
    movne   r2, r2, lsr #12
    addne   r0, r10, r10, lsl #16
    movne   r0, r0, asr #16
    movne   r0, r0, lsl r2

    ldr     r2, [lr, #0x10]     @ phase1
    add     r0, r0, r2
    mov     r0, r0, lsr #16
    lookup_tl r0
    mov     r10,r10,lsl #16     @ ct->op1_out <<= 16;
    mov     r0, r0, lsl #16
    orr     r10,r10, r0, lsr #16

0:
.endm


@ lr=context, r12=pack (stereo, lastchan, disabled, lfo_enabled | pan_r, pan_l, ams[2] | AMmasks[4] | FB[4] | lfo_ampm[16])
@ r0-r2=scratch, r3=sin_tab/scratch, r4=(length<<8)|unused[4],was_update,algo[3], r5=tl_tab/slot,
@ r6-r7=vol_out[4], r8=eg_timer, r9=eg_timer_add[31:16], r10=op1_out, r11=buffer
.global chan_render_loop @ chan_rend_context *ct, int *buffer, int length


.macro chan_render_loop_for_algo algo disabled pan_l pan_r
    stmfd   sp!, {r4-r11,lr}
    mov     lr,  r0
    mov     r4,  r2, lsl #8      @ no more 24 bits here
    ldr     r12, [lr, #0x4c]
    ldr     r0,  [lr, #0x50]
    mov     r11, r1
    and     r0,  r0, #7
    orr     r4,  r4, r0          @ (length<<8)|algo
    add     r0,  lr, #0x44
    ldmia   r0,  {r8,r9}         @ eg_timer, eg_timer_add
    ldr     r10, [lr, #0x54]     @ op1_out
    ldmia   lr,  {r6,r7}         @ load volumes

    tst     r12, #8              @ lfo?
    beq     101f @ crl_loop

100: @ crl_loop_lfo:
    add     r0, lr, #0x30
    ldmia   r0, {r1,r2}

    subs    r4, r4, #0x100
    bmi     105f @ crl_loop_end

    add     r2, r2, r1
    str     r2, [lr, #0x30]

    @ r12=lfo_ampm[31:16], r1=lfo_cnt_old, r2=lfo_cnt
    advance_lfo_m

    add     r4, r4, #0x100

101: @crl_loop:
    subs    r4, r4, #0x100
    bmi     105f @ crl_loop_end

    @ -- EG --
    add     r8, r8, r9
    cmp     r8, #EG_TIMER_OVERFLOW
    bcs     106f @ eg_overflow

103: @eg_done:

.if \disabled == 1
    @ -- disabled? --
    and     r0, r12, #0xC
    cmp     r0, #0xC
    beq     100b @ crl_loop_lfo
    cmp     r0, #0x4
    beq     101b @ crl_loop

.else

    @ -- SLOT1 --
    ldr     r3, =ym_tl_tab

    @ lr=context, r12=pack (stereo, lastchan, disabled, lfo_enabled | pan_r, pan_l, ams[2] | AMmasks[4] | FB[4] | lfo_ampm[16])
    @ r0-r2=scratch, r3=tl_tab, r5=scratch, r6-r7=vol_out[4], r10=op1_out
    upd_slot1_m

    \algo

    @ -- WRITE SAMPLE --
    tst     r0, r0
    beq     104f @ ctl_sample_skip

    @ we don't really care about this in this port:
    @orr     r4, r4, #8              @ have_output

.if \pan_l == 1
    ldr     r1, [r11]
    add     r1, r0, r1
    str     r1, [r11], #4
.else
    add     r11, r11, #4
.endif
.if \pan_r == 1
    ldr     r1, [r11]
    add     r1, r0, r1
    str     r1, [r11], #4
.else
    add     r11, r11, #4
.endif

/*
    tst     r12, #0x20              @ L
    ldrne   r1, [r11]
    addeq   r11, r11, #4
    addne   r1, r0, r1
    strne   r1, [r11], #4
    tst     r12, #0x10              @ R
    ldrne   r1, [r11]
    addeq   r11, r11, #4
    addne   r1, r0, r1
    strne   r1, [r11], #4
*/
    /* duplicated this here to avoid the branch */
    @ -- PHASE UPDATE --
    add     r5, lr, #0x10
    ldmia   r5, {r0-r1}
    add     r5, lr, #0x20
    ldmia   r5, {r2-r3}
    add     r5, lr, #0x10
    add     r0, r0, r2
    add     r1, r1, r3
    stmia   r5!,{r0-r1}
    ldmia   r5, {r0-r1}
    add     r5, lr, #0x28
    ldmia   r5, {r2-r3}
    add     r5, lr, #0x18
    add     r0, r0, r2
    add     r1, r1, r3
    stmia   r5, {r0-r1}

    tst     r12, #8
    bne     100b @ crl_loop_lfo
    b       101b @ crl_loop


104: @ ctl_sample_skip:
    @and     r1, r12, #1
    @add     r1, r1,  #1
    @add     r11,r11, r1, lsl #2
    add     r11,r11, #8             @ assume stereo

    @ -- PHASE UPDATE --
    add     r5, lr, #0x10
    ldmia   r5, {r0-r1}
    add     r5, lr, #0x20
    ldmia   r5, {r2-r3}
    add     r5, lr, #0x10
    add     r0, r0, r2
    add     r1, r1, r3
    stmia   r5!,{r0-r1}
    ldmia   r5, {r0-r1}
    add     r5, lr, #0x28
    ldmia   r5, {r2-r3}
    add     r5, lr, #0x18
    add     r0, r0, r2
    add     r1, r1, r3
    stmia   r5, {r0-r1}

    tst     r12, #8
    bne     100b @ crl_loop_lfo
    b       101b @ crl_loop
.endif

105: @ crl_loop_end:
@    stmia   lr,  {r6,r7}         @ save volumes (for debug)
    str     r8,  [lr, #0x44]     @ eg_timer
    str     r12, [lr, #0x4c]     @ pack (for lfo_ampm)
    str     r4,  [lr, #0x50]     @ was_update
    str     r10, [lr, #0x54]     @ op1_out
    ldmfd   sp!, {r4-r11,pc}

    /* Place the EG overflow code to minimize branch? */
106: @ eg_overflow:
    add     r0, lr, #0x3c
    ldmia   r0, {r1,r5}         @ eg_cnt, CH

102: @eg_loop:
    sub     r8, r8, #EG_TIMER_OVERFLOW
    add     r1, r1, #1
                                        @ SLOT1 (0)
    @ r5=slot, r1=eg_cnt, trashes: r0,r2,r3
    update_eg_phase_slot SLOT1
    add     r5, r5, #SLOT_STRUCT_SIZE*2 @ SLOT2 (2)
    update_eg_phase_slot SLOT2
    sub     r5, r5, #SLOT_STRUCT_SIZE   @ SLOT3 (1)
    update_eg_phase_slot SLOT3
    add     r5, r5, #SLOT_STRUCT_SIZE*2 @ SLOT4 (3)
    update_eg_phase_slot SLOT4

    cmp     r8, #EG_TIMER_OVERFLOW
    subcs   r5, r5, #SLOT_STRUCT_SIZE*3
    bcs     102b @ eg_loop
    str     r1, [lr, #0x3c]
    b       103b @ eg_done

.pool
.endm

/* optimized versions of the chan_render_loop functions (channel enabled) */

.macro chan_render_loop_m algo disabled l r
.global chan_render_loop_algo\algo\()_\disabled\()_\l\r
chan_render_loop_algo\algo\()_\disabled\()_\l\r:
    chan_render_loop_for_algo upd_algo\algo\()_m \disabled \l \r
.endm

.macro chan_render_loop_expand_algo_m disabled l r
chan_render_loop_m 0 \disabled \l \r
chan_render_loop_m 1 \disabled \l \r
chan_render_loop_m 2 \disabled \l \r
chan_render_loop_m 3 \disabled \l \r
chan_render_loop_m 4 \disabled \l \r
chan_render_loop_m 5 \disabled \l \r
chan_render_loop_m 6 \disabled \l \r
chan_render_loop_m 7 \disabled \l \r
.endm

// enabled
chan_render_loop_expand_algo_m 0 0 0 
chan_render_loop_expand_algo_m 0 0 1
chan_render_loop_expand_algo_m 0 1 0 
chan_render_loop_expand_algo_m 0 1 1 

// disabled (actually never used)
chan_render_loop_expand_algo_m 1 0 0 
chan_render_loop_expand_algo_m 1 0 1
chan_render_loop_expand_algo_m 1 1 0 
chan_render_loop_expand_algo_m 1 1 1 


@ vim:filetype=armasm
