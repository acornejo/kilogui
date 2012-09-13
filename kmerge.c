/**
 * Usage: kmerge {control.hex} {program.hex} {output.hex}
 *
 * Merges {control.hex} and {program.hex} and stores the output in
 * {output.hex}. It is almost a direct concatenation, except that the
 * last line of the first file is deleted in the process.
 *
 * @author Alex Cornejo
 * @version 1.0
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char line[512];
    char oldline[512]={'\0'};
    char ch;

    if (argc == 4) {
        FILE *control, *program, *output;

        if ((control=fopen(argv[1],"rb")) == NULL) {
            printf("Error opening \"%s\" for reading\n", argv[1]);
            return -1;
        }

        if ((program=fopen(argv[2],"rb")) == NULL) {
            printf("Error opening \"%s\" for reading\n", argv[2]);
            fclose(control);
            return -1;
        }

        if ((output=fopen(argv[3],"wb")) == NULL) {
            fclose(control);
            fclose(program);
            printf("Error opening \"%s\" for writing\n", argv[3]);
            return -1;
        }

        // copy program except for last line
        while (fgets(line, sizeof(line), program)) {
            if (strchr(line, ':') == NULL) break;
            fputs(oldline,output);
            strcpy(oldline,line);
        }

        // copy control (byte per byte)
        while ((ch=getc(control)) != EOF)
            putc(ch, output);

        fclose(control);
        fclose(program);
        fclose(output);

    } else {
        printf("Usage: kmerge {control.hex} {program.hex} {output.hex}\n");
        return 0;
    }
}
