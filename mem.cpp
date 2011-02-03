#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

const char * MEM_FILE = "/dev/mem";
const int    MAP_PROT = PROT_READ | PROT_WRITE;
const size_t SYS_MAP_SIZE = 0xFFFF+1;
const int SYS_MAP_MASK = 0xFFFF;

class SysMem
{
private:
    int fSysFd;
    unsigned long int fSysMappedMem;
public:
    SysMem()
        : fSysFd(-1) {}
    ~SysMem() { close_mem(); }

public:
    bool open_mem( unsigned long int address ) {
        bool ret = false;
        // Set up memory interface
        fSysFd = open( MEM_FILE, O_RDWR | O_SYNC );
        if (fSysFd >= 0) {
            off_t pgoff;
            pgoff = address & ~SYS_MAP_MASK;
            fSysMappedMem = (unsigned long int)mmap( 0, SYS_MAP_SIZE, MAP_PROT, MAP_SHARED, fSysFd, pgoff );
            ret = (fSysMappedMem != (unsigned long int)MAP_FAILED);
        }
        return ret;
    }
    void close_mem( void ) {
        if (fSysFd >= 0)
            close(fSysFd);
    }

    void write_mem32(unsigned long int addr, unsigned long value) {
        (*(volatile unsigned long *)(fSysMappedMem+(addr&SYS_MAP_MASK))) = value;
    }

    void write_mem16(unsigned long int addr, unsigned short value) {
        (*(volatile unsigned short *)(fSysMappedMem+(addr&SYS_MAP_MASK))) = value;
    }

    unsigned long read_mem32(unsigned long int addr) {
        return (*(volatile unsigned long *)(fSysMappedMem+(addr&SYS_MAP_MASK)));
    }

    unsigned short read_mem16(unsigned long int addr) {
        return (*(volatile unsigned short *)(fSysMappedMem+(addr&SYS_MAP_MASK)));
    }

};

void show_help(const char* prog_name)
{
    fprintf(stderr,"%s --read=address --bits=16|32\n", prog_name);
    fprintf(stderr,"%s --write=address --bits=16|32 --value=value\n", prog_name);
}

int main(int argc, char** argv, char** env)
{
    int ret = 0;
    int c;

    bool iread = false;
    bool iwrite = false;
    bool ivalue = false;
    unsigned long int value = 0;
    unsigned long int bitcount = 0;
    unsigned long int iocount = 1;
    unsigned long int address = 0;

    while (1) {
        static struct option long_options[] = {
            {"write", required_argument, 0, 'w'},
            {"value", required_argument, 0, 'v'},
            {"read", required_argument, 0, 'r'},
            {"bits", required_argument, 0, 'b'},
            {"count", required_argument, 0, 'c'},
            {"help", no_argument, 0, '?'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "w:v:r::cb?",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 'w':
            if (iread) {
                fprintf(stderr, "--read and --write are mutually exclusive\n");
                exit(1);
            }
            iwrite = true;
            address = strtoll(optarg,NULL,0);
            break;

        case 'r':
            if (iwrite) {
                fprintf(stderr, "--read and --write are mutually exclusive\n");
                exit(1);
            }
            iread = true;
            address = strtoll(optarg,NULL,0);
            break;

        case 'v':
            ivalue = true;
            value = strtoll(optarg,NULL,0);
            break;

        case 'b':
            bitcount = strtoll(optarg,NULL,0);
            break;

        case 'c':
            iocount = strtoll(optarg,NULL,0);
            break;

        case '?':
            show_help(argv[0]);
            exit(1);
            break;

        default:
            show_help(argv[0]);
            exit(1);
        }
    }

    if (!(iread || iwrite)) {
        fprintf(stderr,"Need to specify either --read or --write\n");
        show_help(argv[0]);
        exit(1);
    }

    if (bitcount != 16 && bitcount != 32) {
        fprintf(stderr,"Need to specify --bits as either 16 or 32\n");
        show_help(argv[0]);
        exit(1);
    }

    if ((!ivalue && iwrite) || (ivalue && iread)) {
        fprintf(stderr,"Need to specify a value with --value when doing a write only\n");
        show_help(argv[0]);
        exit(1);
    }

    if (iocount * (bitcount/8) > SYS_MAP_SIZE) {
        fprintf(stderr,"--count cannot cause i/o to span more than %u bytes\n", SYS_MAP_SIZE);
        exit(1);
    }

    SysMem mem;

    if (mem.open_mem(address))
    {
        unsigned int i = 0;
        while (i < iocount)
        {
            bool newline = ((i*(bitcount/8))%16)==0;
            if (newline && i > 0)
                printf("\n");
            if (newline)
                printf("0x%08lx:", address);
            if (iread)
            {
                switch (bitcount) {
                case 16:
                    printf(" 0x%04x", mem.read_mem16(address));
                    break;
                case 32:
                    printf(" 0x%08lx", mem.read_mem32(address));
                    break;
                }
            }
            else
            {
                switch (bitcount) {
                case 16:
                    mem.write_mem16(address, value);
                    printf(" 0x%04lx", value);
                    break;
                case 32:
                    mem.write_mem32(address, value);
                    printf(" 0x%08lx",  value);
                    break;
                }
            }
            i++;
            address+=(bitcount/8);
        }
        printf("\n");
    }
    else
    {
        fprintf(stderr, "Opening memory block for 0x%08lx Failed!\n", address);
        ret = 2;
    }

    return ret;
}
