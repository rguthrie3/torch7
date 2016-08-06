#ifndef TH_SIMD_INC
#define TH_SIMD_INC


// Can be found on Intel ISA Reference for CPUID
#define CPUID_AVX2_BIT 0x10       // Bit 5 of EBX for EAX=0x7
#define CPUID_AVX_BIT  0x10000000 // Bit 28 of ECX for EAX=0x1
#define CPUID_SSE_BIT  0x2000000  // bit 25 of EDX for EAX=0x1

// Helper macros for initialization
#define FUNCTION_IMPL(NAME, EXT) \
    { .function=(void *)NAME,    \
      .supportedSimdExt=EXT      \
    }

#define INIT_DISPATCH_PTR(OP)    \
  do {                           \
    for (int i = 0; i < sizeof(THTensor_(dispatchTbl ## OP)) / sizeof(FunctionDescription); ++i) { \
      THTensor_(dispatchPtr ## OP) = THTensor_(dispatchTbl ## OP)[i].function;                     \
      if (THTensor_(dispatchTbl ## OP)[i].supportedSimdExt & hostSimdExts) {                       \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } while(0)


typedef struct FunctionDescription
{
  void *function;
  uint32_t supportedSimdExt;
} FunctionDescription;


enum SIMDExtensions
{
  SIMDExtension_AVX2    = 0x1,
  SIMDExtension_AVX     = 0x2,
  SIMDExtension_SSE     = 0x4,
  SIMDExtension_DEFAULT = 0x0
};


static inline void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
  uint32_t a = *eax, b, c, d;
  asm volatile ( "cpuid\n\t"
                 : "+a"(a), "=b"(b), "+c"(c), "=d"(d) );
  *eax = a;
  *ebx = b;
  *ecx = c;
  *edx = d;
}

static inline uint32_t detectHostSIMDExtensions()
{
  uint32_t eax, ebx, ecx, edx;
  uint32_t hostSimdExts = 0x0;

  // Check for AVX2. Requires separate CPUID
  eax = 0x7;
  cpuid(&eax, &ebx, &ecx, &edx);
  if (ebx & CPUID_AVX2_BIT)
    hostSimdExts |= SIMDExtension_AVX2;

  eax = 0x1;
  cpuid(&eax, &ebx, &ecx, &edx);
  if (ecx & CPUID_AVX_BIT)
    hostSimdExts |= SIMDExtension_AVX;
  if (edx & CPUID_SSE_BIT)
    hostSimdExts |= SIMDExtension_SSE;

  return hostSimdExts;
}

#endif
