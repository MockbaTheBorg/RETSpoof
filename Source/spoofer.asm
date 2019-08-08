PUBLIC _spoofer_stub
 
.code
 
_spoofer_stub PROC
    pop r11
    pop r10
    mov rax, [rsp + 24]
    
    mov r10, [rax]
    mov [rsp], r10
    
    mov r10, [rax + 8]
    mov [rax + 8], r11
 
    mov [rax + 16], rbx
    lea rbx, fixup
    mov [rax], rbx
    mov rbx, rax
    
    push r10					; Branches onto the spoofed function
	ret
 
fixup:							; Return point from the spoofed function
    sub rsp, 16
    mov rcx, rbx
    mov rbx, [rcx + 16]
    jmp QWORD PTR [rcx + 8]
_spoofer_stub ENDP
 
END
