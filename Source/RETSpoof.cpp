#include "RETSpoof.h"

uint64_t jmppoint;

// This function exists only to give some extra code to the test
// before the function we are going to spoof
void Separator() {
	printf("---------------------------------------\n");
}

// This function exists only to be used once and overwritten by
// our JMP RBX instruction
void PrintHeader() {
	printf("RET Address spoof - v1.0\n");
}

// This is the original game function, it must be called from
// inside the game memory block only, otherwise BAN!
uint64_t GameFunction(uint64_t native, uint64_t number, const char* text) {
	Separator();
	printf("This is the game function speaking:\n");

	// Anti-Cheat check (some dummy check actually in this case)
	const auto ret = _ReturnAddress();
	printf("(Anti Cheat code running)\n");
	printf("GameFunction return address is 0x%llx.\n", (uint64_t)ret);
	if (ret == (void*)&PrintHeader) {
		printf("RET = 0x%llx ... all good ... proceed.\n", (uint64_t)ret);
	} else {
		printf("RET 0x%llx != 0x%llx. You've been banned.\n", (uint64_t)ret, (uint64_t)&PrintHeader);
	}
	//

	printf("The data passed was 0x%llx, 0x%llx, %s\n", native, number, text);
	return(native + number);
}

void PrintLine(int n) {
	for(int i = 0; i < n; i++)
		printf("-");
	printf("\n");
}

// This is a stub for doing the detour
static uint64_t(*TrueGameFunction)(uint64_t, uint64_t, const char*);

// This is my hacked GameFunction, it will actually exist inside
// the proxied or injected .DLL (memory area different than the
// game's memory)
uint64_t MyGameFunction(uint64_t native, uint64_t number, const char* text) {
	Separator();
	printf("This is my hacked function speaking:\n");

	const auto ret = _ReturnAddress();
	printf("My GameFunction return address is 0x%llx.\n", (uint64_t)ret);

	// Now we do the dummy hacking code
	printf("This is my function doing its thing...\n");
	printf("Then spoof calling the true game function...\n");

	// Instead of seeing its return address as being here, the GameFunction
	// will see its return address as being jmppoint (JMP RBX), and then
	// jmppoint jumps back here.
	spoof_call((void*)jmppoint, PrintLine, 10);
	return(spoof_call((void*)jmppoint, TrueGameFunction, native, number, "My hacked text"));
}

// Detours Hooking frond end (to keep same syntax as MinHook)
bool HookFunction(uint64_t pAddress, void *pDetour, void** ppOriginal) {
	*ppOriginal = (void*)pAddress;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	long errorCode = DetourAttach((PVOID*)ppOriginal, (PVOID)pDetour);
	DetourTransactionCommit();
	if (errorCode != NO_ERROR) {
		printf("Error %ld while hooking!\n", errorCode);
	}
	return errorCode == NO_ERROR;
}

// This is the Main Game Code. We will use to call our function as
// well, it doesn't matter, as the return address will be spoofed
// anyways.
int main() {
	PrintHeader();
	// The original addresses for the functions, just for information
	printf("This is MAIN speaking:\n");
	printf("PrintHeader Function is at 0x%llx.\n", (uint64_t)&PrintHeader);
	printf("Original Game Function is at 0x%llx.\n", (uint64_t)&GameFunction);
	printf("Detoured Game Function is at 0x%llx.\n", (uint64_t)&MyGameFunction);
	printf("Stub Game Function is at 0x%llx.\n", (uint64_t)&TrueGameFunction);
	Separator();

	// Detour the Game Function
	HookFunction((uint64_t)&GameFunction, (LPVOID)&MyGameFunction, (LPVOID*)&TrueGameFunction);

	// Patch the JMP RBX code onto an unused game area (PrintHeader ran already)
	jmppoint = (uint64_t)&PrintHeader;
	DWORD op;
	VirtualProtect((void*)jmppoint, 8, PAGE_EXECUTE_READWRITE, &op);
	memset((void*)(jmppoint+0), 0xFF, 1);	// JMP
	memset((void*)(jmppoint+1), 0x23, 1);	// RBX0
	printf("JMP RBX is now at 0x%llx.\n", (uint64_t)jmppoint);

	// Call the (now detoured) Game Function
	printf("Game is calling its function...\n");
	uint64_t whatevs = GameFunction(0xDEAD000000000000, 0xC0DE00000000, "Game's Text");
	printf("Result is 0x%llx.\n", whatevs);
}