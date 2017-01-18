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
		lock and size ptr [rcx], value
		ret
	name endp
endm

define_and_assign embb_internal__atomic_and_assign_8_asm, qword, rdx
define_and_assign embb_internal__atomic_and_assign_4_asm, dword, edx
define_and_assign embb_internal__atomic_and_assign_2_asm, word, dx
define_and_assign embb_internal__atomic_and_assign_1_asm, byte, dl

; ---- compare_and_swap -------------------------------------------------------

define_compare_and_swap macro name, size, value, desired
	public name
	name proc
		mov desired, size ptr [rdx]
		lock cmpxchg size ptr [rcx], value
		mov size ptr [rdx], desired
		setz al
		movzx rax, al
		ret
	name endp
endm

define_compare_and_swap embb_internal__atomic_compare_and_swap_8_asm, qword, r8, rax
define_compare_and_swap embb_internal__atomic_compare_and_swap_4_asm, dword, r8d, eax
define_compare_and_swap embb_internal__atomic_compare_and_swap_2_asm, word, r8w, ax
define_compare_and_swap embb_internal__atomic_compare_and_swap_1_asm, byte, r8b, al

; ---- fetch_and_add ----------------------------------------------------------

define_fetch_and_add macro name, size, value, return
	public name
	name proc
		lock xadd size ptr [rcx], value
		mov return, value
		ret
	name endp
endm

define_fetch_and_add embb_internal__atomic_fetch_and_add_8_asm, qword, rdx, rax
define_fetch_and_add embb_internal__atomic_fetch_and_add_4_asm, dword, edx, eax
define_fetch_and_add embb_internal__atomic_fetch_and_add_2_asm, word, dx, ax
define_fetch_and_add embb_internal__atomic_fetch_and_add_1_asm, byte, dl, al

; ---- load -------------------------------------------------------------------

define_load macro name, size, return
	public name
	name proc
		mov return, size ptr [rcx]
		ret
	name endp
endm

define_load embb_internal__atomic_load_8_asm, qword, rax
define_load embb_internal__atomic_load_4_asm, dword, eax
define_load embb_internal__atomic_load_2_asm, word, ax
define_load embb_internal__atomic_load_1_asm, byte, al

; ---- memory_barrier ---------------------------------------------------------

define_memory_barrier macro name
	public name
	name proc
		mfence
		ret
	name endp
endm

define_memory_barrier embb_internal__atomic_memory_barrier_asm

; ---- or_assign --------------------------------------------------------------

define_or_assign macro name, size, value
	public name
	name proc
		lock or size ptr [rcx], value
		ret
	name endp
endm

define_or_assign embb_internal__atomic_or_assign_8_asm, qword, rdx
define_or_assign embb_internal__atomic_or_assign_4_asm, dword, edx
define_or_assign embb_internal__atomic_or_assign_2_asm, word, dx
define_or_assign embb_internal__atomic_or_assign_1_asm, byte, dl

; ---- store ------------------------------------------------------------------

define_store macro name, size, value
	public name
	name proc
		lock xchg size ptr [rcx], value
		ret
	name endp
endm

define_store embb_internal__atomic_store_8_asm, qword, rdx
define_store embb_internal__atomic_store_4_asm, dword, edx
define_store embb_internal__atomic_store_2_asm, word, dx
define_store embb_internal__atomic_store_1_asm, byte, dl

; ---- swap -------------------------------------------------------------------

define_swap macro name, size, value, return
	public name
	name proc
		lock xchg size ptr [rcx], value
		mov return, value
		ret
	name endp
endm

define_swap embb_internal__atomic_swap_8_asm, qword, rdx, rax
define_swap embb_internal__atomic_swap_4_asm, dword, edx, eax
define_swap embb_internal__atomic_swap_2_asm, word, dx, ax
define_swap embb_internal__atomic_swap_1_asm, byte, dl, al

; ---- xor_assign -------------------------------------------------------------

define_xor_assign macro name, size, value
	public name
	name proc
		lock xor size ptr [rcx], value
		ret
	name endp
endm

define_xor_assign embb_internal__atomic_xor_assign_8_asm, qword, rdx
define_xor_assign embb_internal__atomic_xor_assign_4_asm, dword, edx
define_xor_assign embb_internal__atomic_xor_assign_2_asm, word, dx
define_xor_assign embb_internal__atomic_xor_assign_1_asm, byte, dl

_TEXT	ENDS

END
