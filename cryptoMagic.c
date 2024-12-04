#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
void encryptFile(const char *inputFileName);
void decryptFile(const char *inputFileName);
void getNewFileName(const char *inputFileName, const char *newExtension, char *newFileName);

int main(int argc, char *argv[]) {
    int mode = 0; // Default: Encryption mode (0 for encrypt, 1 for decrypt)
    const char *inputFileName;

    // Parse command-line arguments
    if (argc == 2) {
        inputFileName = argv[1]; // Default to encryption if only filename provided
    } else if (argc == 3) {
        if (strcmp(argv[1], "-E") == 0) {
            mode = 0; // Encryption mode
        } else if (strcmp(argv[1], "-D") == 0) {
            mode = 1; // Decryption mode
        } else {
            printf("Invalid option. Use -E for encryption or -D for decryption.\n");
            return 1;
        }
        inputFileName = argv[2];
    } else {
        printf("Usage: %s [-E|-D] <filename>\n", argv[0]);
        return 1;
    }

    // Call the appropriate function
    if (mode == 0) {
        encryptFile(inputFileName);
    } else {
        decryptFile(inputFileName);
    }
    return 0;
}

// Function to create a new file name by changing its extension
void getNewFileName(const char *inputFileName, const char *newExtension, char *newFileName) {
    const char *dot = strrchr(inputFileName, '.'); // Find the last '.' in the filename
    if (dot) {
        snprintf(newFileName, dot - inputFileName + 1, "%s", inputFileName); // Copy up to the dot
    } else {
        strcpy(newFileName, inputFileName); // No dot found, copy the entire name
    }
    strcat(newFileName, newExtension); // Add the new extension
}

// Function to encrypt a file
void encryptFile(const char *inputFileName) {
    FILE *inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        printf("Error: Could not open file %s for reading.\n", inputFileName);
        return;
    }

    char outputFileName[256];
    getNewFileName(inputFileName, ".crp", outputFileName);
    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("Error: Could not create file %s.\n", outputFileName);
        fclose(inputFile);
        return;
    }

    char ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        int encryptedChar = ch - 16;
        if (encryptedChar < 32) {
            encryptedChar = (encryptedChar - 32) + 144;
        }
        fprintf(outputFile, "%02X", encryptedChar);
    }

    fclose(inputFile);
    fclose(outputFile);
    printf("Encryption complete. Encrypted file: %s\n", outputFileName);
}

// Function to decrypt a file
void decryptFile(const char *inputFileName) {
    FILE *inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        printf("Error: Could not open file %s for reading.\n", inputFileName);
        return;
    }

    char outputFileName[256];
    getNewFileName(inputFileName, ".txt", outputFileName);
    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("Error: Could not create file %s.\n", outputFileName);
        fclose(inputFile);
        return;
    }

    char buffer[3] = {0}; // Buffer for 2-character hex codes
    while (fscanf(inputFile, "%2s", buffer) == 1) {
        printf("Read hex: %s\n", buffer); // Debugging
        int decryptedChar = (int)strtol(buffer, NULL, 16);
        printf("Decrypted int: %d\n", decryptedChar); // Debugging
        decryptedChar += 16;
        if (decryptedChar > 127) {
            decryptedChar = (decryptedChar - 144) + 32;
        }
        printf("Final decrypted char: %c\n", decryptedChar); // Debugging
        fputc(decryptedChar, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("Decryption complete. Decrypted file: %s\n", outputFileName);
}
