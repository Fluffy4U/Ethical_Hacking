// PasswdCheck.cpp : Defines the entry point for the console application.
//

#include <string.h>
#include <Windows.h>
#include <stdio.h>

#define CH_BUFF_SIZE 16 

class PasswdCheck {
private:
    char buffer[CH_BUFF_SIZE];
    char string[CH_BUFF_SIZE];

public:
    int ok;

    PasswdCheck() : ok(1) {
        // Initialize buffers to zero to prevent information leakage
        memset(buffer, 0, CH_BUFF_SIZE);
        memset(string, 0, CH_BUFF_SIZE);
    }

    void CheckPass(int argc, char* argv[]) {
        // FIX 1: Use _snprintf_s instead of sprintf.
        // This stops the overflow seen in image_aa5972.png by enforcing the 16-byte limit.
        // _TRUNCATE ensures the string is null-terminated within the buffer size.
        if (_snprintf_s(string, CH_BUFF_SIZE, _TRUNCATE, "%s %s", argv[2], argv[3]) < 0) {
            // Treat truncated or overly long input as an automatic failure
            ok = 1;
            return;
        }

        // FIX 2: Use fopen_s as suggested by the compiler warning (image_b4d9bd.png).
        FILE* in = nullptr;
        errno_t err = fopen_s(&in, argv[1], "rb");
        if (err != 0 || !in) {
            ok = -1;
            return;
        }

        // FIX 3: Read exactly CH_BUFF_SIZE to avoid boundary manipulation (image_b4ce00.png).
        while (ok && fgets(buffer, CH_BUFF_SIZE, in)) {

            // Clean up the buffer by removing newline characters
            buffer[strcspn(buffer, "\r\n")] = 0;

            // FIX 4: Use strcmp instead of strstr.
            // This stops the logic exploit in image_9afb3e.png by requiring an EXACT match.
            // Unlike strstr, strcmp ensures "admin a" cannot match "admin admin".
            if (strcmp(buffer, string) == 0) {
                ok = 0;
            }
        }

        if (in) fclose(in);
    }
};

// main function which start the program
int main(int argc, char* argv[])
{
	// check valid command line input
	if(argc != 4) {
		printf("Run syntax: buffertest.exe <password database filename> <user login> <user password> \n"
			"Return values: 1-denied, 0-accessed, (other)-system error");
		return -1; // not enough parameters, system error
	}

	// create CheckPass object
	PasswdCheck pc;

	// call CheckPass with number of arguments and arguments character array
	pc.CheckPass(argc, argv);

	if(pc.ok == 0) {
		printf("Access granted for user: %s", argv[2]);
		OutputDebugString(L"Access granted!");
	}
	else if (pc.ok < 0) {
		printf("Database not found! Full path to file is expected.");
		OutputDebugString(L"Wrong database!");
	}
	else {
		printf("Access denied, username or password incorrect!");
		OutputDebugString(L"Access denied!");
	}
	// return code for the program to OS
	return pc.ok;
}
