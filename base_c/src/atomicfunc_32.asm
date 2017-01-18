	.686p
	.xmm
	.model flat

_TEXT	SEGMENT

_abort proto

define_not_implemented macro name
	public name
	name proc
		call _abort
		ret
	name endp
endm

; ---- and_assign -------------------------------------------------------------

define_and_assign macro name, size, value
	public name
	name proc
		lock and size ptr [ecx], value
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_and_assign_8_asm@8
define_and_assign @embb_internal__atomic_and_assign_4_asm@8, dword, edx
define_and_assign @embb_internal__atomic_and_assign_2_asm@8, word, dx
define_and_assign @embb_internal__atomic_and_assign_1_asm@8, byte, dl

; ---- compare_and_swap -------------------------------------------------------

define_compare_and_swap macro name, size, value, desired
	public name
	name proc
		push edx
		mov desired, size ptr [edx]
		mov value, size ptr [esp+8]
		lock cmpxchg size ptr [ecx], value
		pop edx
		mov size ptr [edx], desired
		setz al
		movzx eax, al
		ret 4
	name endp
endm

define_not_implemented @embb_internal__atomic_compare_and_swap_8_asm@12
define_compare_and_swap @embb_internal__atomic_compare_and_swap_4_asm@12, dword, edx, eax
define_compare_and_swap @embb_internal__atomic_compare_and_swap_2_asm@12, word, dx, ax
define_compare_and_swap @embb_internal__atomic_compare_and_swap_1_asm@12, byte, dl, al

; ---- fetch_and_add ----------------------------------------------------------

define_fetch_and_add macro name, size, value, return
	public name
	name proc
		lock xadd size ptr [ecx], value
		mov return, value
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_fetch_and_add_8_asm@8
define_fetch_and_add @embb_internal__atomic_fetch_and_add_4_asm@8, dword, edx, eax
define_fetch_and_add @embb_internal__atomic_fetch_and_add_2_asm@8, word, dx, ax
define_fetch_and_add @embb_internal__atomic_fetch_and_add_1_asm@8, byte, dl, al

; ---- load -------------------------------------------------------------------

define_load macro name, size, return
	public name
	name proc
		mov return, size ptr [ecx]
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_load_8_asm@4
define_load @embb_internal__atomic_load_4_asm@4, dword, eax
define_load @embb_internal__atomic_load_2_asm@4, word, ax
define_load @embb_internal__atomic_load_1_asm@4, byte, al

; ---- memory_barrier ---------------------------------------------------------

define_memory_barrier macro name
	public name
	name proc
		mfence
		ret
	name endp
endm

define_memory_barrier @embb_internal__atomic_memory_barrier_asm@0

; ---- or_assign --------------------------------------------------------------

define_or_assign macro name, size, value
	public name
	name proc
		lock or size ptr [ecx], value
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_or_assign_8_asm@8
define_or_assign @embb_internal__atomic_or_assign_4_asm@8, dword, edx
define_or_assign @embb_internal__atomic_or_assign_2_asm@8, word, dx
define_or_assign @embb_internal__atomic_or_assign_1_asm@8, byte, dl

; ---- store ------------------------------------------------------------------

define_store macro name, size, value
	public name
	name proc
		lock xchg size ptr [ecx], value
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_store_8_asm@8
define_store @embb_internal__atomic_store_4_asm@8, dword, edx
define_store @embb_internal__atomic_store_2_asm@8, word, dx
define_store @embb_internal__atomic_store_1_asm@8, byte, dl

; ---- swap -------------------------------------------------------------------

define_swap macro name, size, value, return
	public name
	name proc
		lock xchg size ptr [ecx], value
		mov return, value
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_swap_8_asm@8
define_swap @embb_internal__atomic_swap_4_asm@8, dword, edx, eax
define_swap @embb_internal__atomic_swap_2_asm@8, word, dx, ax
define_swap @embb_internal__atomic_swap_1_asm@8, byte, dl, al

; ---- xor_assign -------------------------------------------------------------

define_xor_assign macro name, size, value
	public name
	name proc
		lock xor size ptr [ecx], value
		ret
	name endp
endm

define_not_implemented @embb_internal__atomic_xor_assign_8_asm@8
define_xor_assign @embb_internal__atomic_xor_assign_4_asm@8, dword, edx
define_xor_assign @embb_internal__atomic_xor_assign_2_asm@8, word, dx
define_xor_assign @embb_internal__atomic_xor_assign_1_asm@8, byte, dl

_TEXT	ENDS

END
