// [AsmJit]
// Complete x86/x64 JIT and Remote Assembler for C++.
//
// [License]
// Zlib - See LICENSE.md file in the package.

// [Guard]
#ifndef _ASMJIT_X86_X86OPERAND_H
#define _ASMJIT_X86_X86OPERAND_H

// [Dependencies]
#include "../base/assembler.h"
#include "../base/compiler.h"
#include "../base/operand.h"
#include "../base/utils.h"
#include "../base/vectypes.h"

// [Api-Begin]
#include "../apibegin.h"

//! \internal
//!
//! Internal macro to get an operand ID casting it to `Operand`. Basically
//! allows to get an id of operand that has been just 'typedef'ed.
#define ASMJIT_OP_ID(_Op_) reinterpret_cast<const Operand&>(_Op_).getId()

namespace asmjit {

// ============================================================================
// [Forward Declarations]
// ============================================================================

class X86Reg;
class X86GpReg;
class X86FpReg;
class X86MmReg;
class X86KReg;
class X86XmmReg;
class X86YmmReg;
class X86ZmmReg;

class X86SegReg;
class X86RipReg;

#if !defined(ASMJIT_DISABLE_COMPILER)
class X86Var;
class X86GpVar;
class X86MmVar;
class X86KVar;
class X86XmmVar;
class X86YmmVar;
class X86ZmmVar;
#endif // !ASMJIT_DISABLE_COMPILER

//! \addtogroup asmjit_x86
//! \{

// ============================================================================
// [asmjit::X86RegClass]
// ============================================================================

//! X86/X64 register class.
ASMJIT_ENUM(X86RegClass) {
  // --------------------------------------------------------------------------
  // [Regs & Vars]
  // --------------------------------------------------------------------------

  //! X86/X64 Gp register class (compatible with universal \ref kRegClassGp).
  kX86RegClassGp = kRegClassGp,
  //! X86/X64 Mm register class.
  kX86RegClassMm = 1,
  //! X86/X64 K register class.
  kX86RegClassK = 2,
  //! X86/X64 XMM/YMM/ZMM register class.
  kX86RegClassXyz = 3,

  //! \internal
  //!
  //! Last register class that is managed by `X86Compiler`, used by asserts.
  _kX86RegClassManagedCount = 4,

  // --------------------------------------------------------------------------
  // [Regs Only]
  // --------------------------------------------------------------------------

  //! X86/X64 Fp register class.
  kX86RegClassFp = 4,

  //! Count of X86/X64 register classes.
  kX86RegClassCount = 5
};

// ============================================================================
// [asmjit::X86RegType]
// ============================================================================

//! X86/X64 register type.
ASMJIT_ENUM(X86RegType) {
  //! Low GPB register (AL, BL, CL, DL, ...).
  kX86RegTypeGpbLo = 0x01,
  //! High GPB register (AH, BH, CH, DH only).
  kX86RegTypeGpbHi = 0x02,

  //! \internal
  //!
  //! High GPB register patched to a native index (4-7).
  _kX86RegTypePatchedGpbHi = kX86RegTypeGpbLo | kX86RegTypeGpbHi,

  //! GPW register.
  kX86RegTypeGpw = 0x10,
  //! GPD register.
  kX86RegTypeGpd = 0x20,
  //! GPQ register (X64).
  kX86RegTypeGpq = 0x30,

  //! FPU register.
  kX86RegTypeFp = 0x40,
  //! MMX register (MMX+).
  kX86RegTypeMm = 0x50,

  //! K register (AVX512+).
  kX86RegTypeK = 0x60,

  //! XMM register (SSE+).
  kX86RegTypeXmm = 0x70,
  //! YMM register (AVX+).
  kX86RegTypeYmm = 0x80,
  //! ZMM register (AVX512+).
  kX86RegTypeZmm = 0x90,

  //! Instruction pointer (RIP).
  kX86RegTypeRip = 0xE0,
  //! Segment register.
  kX86RegTypeSeg = 0xF0
};

// ============================================================================
// [asmjit::X86RegIndex]
// ============================================================================

//! X86/X64 register indexes.
//!
//! NOTE: Register indexes have been reduced to only support general purpose
//! registers. There is no need to have enumerations with number suffix that
//! expands to the exactly same value as the suffix value itself.
ASMJIT_ENUM(X86RegIndex) {
  //! Index of Al/Ah/Ax/Eax/Rax registers.
  kX86RegIndexAx = 0,
  //! Index of Cl/Ch/Cx/Ecx/Rcx registers.
  kX86RegIndexCx = 1,
  //! Index of Dl/Dh/Dx/Edx/Rdx registers.
  kX86RegIndexDx = 2,
  //! Index of Bl/Bh/Bx/Ebx/Rbx registers.
  kX86RegIndexBx = 3,
  //! Index of Spl/Sp/Esp/Rsp registers.
  kX86RegIndexSp = 4,
  //! Index of Bpl/Bp/Ebp/Rbp registers.
  kX86RegIndexBp = 5,
  //! Index of Sil/Si/Esi/Rsi registers.
  kX86RegIndexSi = 6,
  //! Index of Dil/Di/Edi/Rdi registers.
  kX86RegIndexDi = 7,
  //! Index of R8b/R8w/R8d/R8 registers (64-bit only).
  kX86RegIndexR8 = 8,
  //! Index of R9B/R9w/R9d/R9 registers (64-bit only).
  kX86RegIndexR9 = 9,
  //! Index of R10B/R10w/R10D/R10 registers (64-bit only).
  kX86RegIndexR10 = 10,
  //! Index of R11B/R11w/R11d/R11 registers (64-bit only).
  kX86RegIndexR11 = 11,
  //! Index of R12B/R12w/R12d/R12 registers (64-bit only).
  kX86RegIndexR12 = 12,
  //! Index of R13B/R13w/R13d/R13 registers (64-bit only).
  kX86RegIndexR13 = 13,
  //! Index of R14B/R14w/R14d/R14 registers (64-bit only).
  kX86RegIndexR14 = 14,
  //! Index of R15B/R15w/R15d/R15 registers (64-bit only).
  kX86RegIndexR15 = 15
};

// ============================================================================
// [asmjit::X86Seg]
// ============================================================================

//! X86/X64 segment codes.
ASMJIT_ENUM(X86Seg) {
  //! No/Default segment.
  kX86SegDefault = 0,
  //! Es segment.
  kX86SegEs = 1,
  //! Cs segment.
  kX86SegCs = 2,
  //! Ss segment.
  kX86SegSs = 3,
  //! Ds segment.
  kX86SegDs = 4,
  //! Fs segment.
  kX86SegFs = 5,
  //! Gs segment.
  kX86SegGs = 6,

  //! Count of X86 segment registers supported by AsmJit.
  //!
  //! NOTE: X86 architecture has 6 segment registers - ES, CS, SS, DS, FS, GS.
  //! X64 architecture lowers them down to just FS and GS. AsmJit supports 7
  //! segment registers - all addressable in both X86 and X64 modes and one
  //! extra called `kX86SegDefault`, which is AsmJit specific and means that there
  //! is no segment register specified so the segment prefix will not be emitted.
  kX86SegCount = 7
};

// ============================================================================
// [asmjit::X86MemVSib]
// ============================================================================

//! X86/X64 index register legacy and AVX2 (VSIB) support.
ASMJIT_ENUM(X86MemVSib) {
  //! Memory operand uses GPD/GPQ index (or no index register).
  kX86MemVSibGpz = 0,
  //! Memory operand uses XMM index (or no index register).
  kX86MemVSibXmm = 1,
  //! Memory operand uses YMM index (or no index register).
  kX86MemVSibYmm = 2,
  //! Memory operand uses ZMM index (or no index register).
  kX86MemVSibZmm = 3
};

// ============================================================================
// [asmjit::X86MemFlags]
// ============================================================================

//! \internal
//!
//! X86/X64 specific memory flags.
ASMJIT_ENUM(X86MemFlags) {
  kX86MemSegBits    = 0x7,
  kX86MemSegIndex   = 0,
  kX86MemSegMask    = kX86MemSegBits << kX86MemSegIndex,

  kX86MemGpdBits    = 0x1,
  kX86MemGpdIndex   = 3,
  kX86MemGpdMask    = kX86MemGpdBits << kX86MemGpdIndex,

  kX86MemVSibBits   = 0x3,
  kX86MemVSibIndex  = 4,
  kX86MemVSibMask   = kX86MemVSibBits << kX86MemVSibIndex,

  kX86MemShiftBits  = 0x3,
  kX86MemShiftIndex = 6,
  kX86MemShiftMask  = kX86MemShiftBits << kX86MemShiftIndex
};

// ============================================================================
// [asmjit::X86VarType]
// ============================================================================

//! X86/X64 variable type.
ASMJIT_ENUM(X86VarType) {
  //! Variable is SP-FP (FPU).
  kX86VarTypeFp32 = kVarTypeFp32,
  //! Variable is DP-FP (FPU).
  kX86VarTypeFp64 = kVarTypeFp64,

  //! Variable is MMX (MMX).
  kX86VarTypeMm = 12,

  //! Variable is K (AVX512+)
  kX86VarTypeK,

  //! Variable is XMM (SSE+).
  kX86VarTypeXmm,
  //! Variable is a scalar XMM SP-FP number.
  kX86VarTypeXmmSs,
  //! Variable is a packed XMM SP-FP number (4 floats).
  kX86VarTypeXmmPs,
  //! Variable is a scalar XMM DP-FP number.
  kX86VarTypeXmmSd,
  //! Variable is a packed XMM DP-FP number (2 doubles).
  kX86VarTypeXmmPd,

  //! Variable is YMM (AVX+).
  kX86VarTypeYmm,
  //! Variable is a packed YMM SP-FP number (8 floats).
  kX86VarTypeYmmPs,
  //! Variable is a packed YMM DP-FP number (4 doubles).
  kX86VarTypeYmmPd,

  //! Variable is ZMM (AVX512+).
  kX86VarTypeZmm,
  //! Variable is a packed ZMM SP-FP number (16 floats).
  kX86VarTypeZmmPs,
  //! Variable is a packed ZMM DP-FP number (8 doubles).
  kX86VarTypeZmmPd,

  //! Count of variable types.
  kX86VarTypeCount,

  //! \internal
  //! \{
  _kX86VarTypeMmStart = kX86VarTypeMm,
  _kX86VarTypeMmEnd = kX86VarTypeMm,

  _kX86VarTypeXmmStart = kX86VarTypeXmm,
  _kX86VarTypeXmmEnd = kX86VarTypeXmmPd,

  _kX86VarTypeYmmStart = kX86VarTypeYmm,
  _kX86VarTypeYmmEnd = kX86VarTypeYmmPd,

  _kX86VarTypeZmmStart = kX86VarTypeZmm,
  _kX86VarTypeZmmEnd = kX86VarTypeZmmPd
  //! \}
};

// ============================================================================
// [asmjit::X86RegCount]
// ============================================================================

//! \internal
//!
//! X86/X64 registers count.
//!
//! Since the number of registers changed across CPU generations `X86RegCount`
//! class is used by `X86Assembler` and `X86Compiler` to provide a way to get
//! number of available registers dynamically. 32-bit mode offers always only
//! 8 registers of all classes, however, 64-bit mode offers 16 Gp registers and
//! 16 XMM/YMM/ZMM registers. AVX512 instruction set doubles the number of SIMD
//! registers (XMM/YMM/ZMM) to 32, this mode has to be explicitly enabled to
//! take effect as it changes some assumptions.
//!
//! `X86RegCount` is also used extensively by `X86Compiler`'s register allocator
//! and data structures. Fp registers were omitted as they are never mapped to
//! variables, thus, not needed to be managed.
//!
//! NOTE: At the moment `X86RegCount` can fit into 32-bits, having 8-bits for
//! each register class except `fp`. This can change in the future after a
//! new instruction set, which adds more registers, is introduced.
struct X86RegCount {
  // --------------------------------------------------------------------------
  // [Zero]
  // --------------------------------------------------------------------------

  //! Reset all counters to zero.
  ASMJIT_INLINE void reset() noexcept { _packed = 0; }

  // --------------------------------------------------------------------------
  // [Get]
  // --------------------------------------------------------------------------

  //! Get register count by a register class `rc`.
  ASMJIT_INLINE uint32_t get(uint32_t rc) const noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    uint32_t shift = Utils::byteShiftOfDWordStruct(rc);
    return (_packed >> shift) & static_cast<uint32_t>(0xFF);
  }

  //! Get Gp count.
  ASMJIT_INLINE uint32_t getGp() const noexcept { return get(kX86RegClassGp); }
  //! Get Mm count.
  ASMJIT_INLINE uint32_t getMm() const noexcept { return get(kX86RegClassMm); }
  //! Get K count.
  ASMJIT_INLINE uint32_t getK() const noexcept { return get(kX86RegClassK); }
  //! Get XMM/YMM/ZMM count.
  ASMJIT_INLINE uint32_t getXyz() const noexcept { return get(kX86RegClassXyz); }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  //! Set register count by a register class `rc`.
  ASMJIT_INLINE void set(uint32_t rc, uint32_t n) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);
    ASMJIT_ASSERT(n <= 0xFF);

    uint32_t shift = Utils::byteShiftOfDWordStruct(rc);
    _packed = (_packed & ~static_cast<uint32_t>(0xFF << shift)) + (n << shift);
  }

  //! Set Gp count.
  ASMJIT_INLINE void setGp(uint32_t n) noexcept { set(kX86RegClassGp, n); }
  //! Set Mm count.
  ASMJIT_INLINE void setMm(uint32_t n) noexcept { set(kX86RegClassMm, n); }
  //! Set K count.
  ASMJIT_INLINE void setK(uint32_t n) noexcept { set(kX86RegClassK, n); }
  //! Set XMM/YMM/ZMM count.
  ASMJIT_INLINE void setXyz(uint32_t n) noexcept { set(kX86RegClassXyz, n); }

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  //! Add register count by a register class `rc`.
  ASMJIT_INLINE void add(uint32_t rc, uint32_t n = 1) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);
    ASMJIT_ASSERT(0xFF - static_cast<uint32_t>(_regs[rc]) >= n);

    uint32_t shift = Utils::byteShiftOfDWordStruct(rc);
    _packed += n << shift;
  }

  //! Add GP count.
  ASMJIT_INLINE void addGp(uint32_t n) noexcept { add(kX86RegClassGp, n); }
  //! Add MMX count.
  ASMJIT_INLINE void addMm(uint32_t n) noexcept { add(kX86RegClassMm, n); }
  //! Add K count.
  ASMJIT_INLINE void addK(uint32_t n) noexcept { add(kX86RegClassK, n); }
  //! Add XMM/YMM/ZMM count.
  ASMJIT_INLINE void addXyz(uint32_t n) noexcept { add(kX86RegClassXyz, n); }

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  //! Build register indexes based on the given `count` of registers.
  ASMJIT_INLINE void indexFromRegCount(const X86RegCount& count) noexcept {
    uint32_t x = static_cast<uint32_t>(count._regs[0]);
    uint32_t y = static_cast<uint32_t>(count._regs[1]) + x;
    uint32_t z = static_cast<uint32_t>(count._regs[2]) + y;

    ASMJIT_ASSERT(y <= 0xFF);
    ASMJIT_ASSERT(z <= 0xFF);
    _packed = Utils::pack32_4x8(0, x, y, z);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union {
    struct {
      //! Count of GP registers.
      uint8_t _gp;
      //! Count of MMX registers.
      uint8_t _mm;
      //! Count of K registers.
      uint8_t _k;
      //! Count of XMM/YMM/ZMM registers.
      uint8_t _xyz;
    };

    uint8_t _regs[4];
    uint32_t _packed;
  };
};

// ============================================================================
// [asmjit::X86RegMask]
// ============================================================================

//! \internal
//!
//! X86/X64 registers mask.
struct X86RegMask {
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! Reset all register masks to zero.
  ASMJIT_INLINE void reset() noexcept {
    _packed.reset();
  }

  // --------------------------------------------------------------------------
  // [IsEmpty / Has]
  // --------------------------------------------------------------------------

  //! Get whether all register masks are zero (empty).
  ASMJIT_INLINE bool isEmpty() const noexcept {
    return _packed.isZero();
  }

  ASMJIT_INLINE bool has(uint32_t rc, uint32_t mask = 0xFFFFFFFF) const noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : return (static_cast<uint32_t>(_gp ) & mask) != 0;
      case kX86RegClassMm : return (static_cast<uint32_t>(_mm ) & mask) != 0;
      case kX86RegClassK  : return (static_cast<uint32_t>(_k  ) & mask) != 0;
      case kX86RegClassXyz: return (static_cast<uint32_t>(_xyz) & mask) != 0;
    }

    return false;
  }

  ASMJIT_INLINE bool hasGp(uint32_t mask = 0xFFFFFFFF) const noexcept { return has(kX86RegClassGp, mask); }
  ASMJIT_INLINE bool hasMm(uint32_t mask = 0xFFFFFFFF) const noexcept { return has(kX86RegClassMm, mask); }
  ASMJIT_INLINE bool hasK(uint32_t mask = 0xFFFFFFFF) const noexcept { return has(kX86RegClassK, mask); }
  ASMJIT_INLINE bool hasXyz(uint32_t mask = 0xFFFFFFFF) const noexcept { return has(kX86RegClassXyz, mask); }

  // --------------------------------------------------------------------------
  // [Get]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE uint32_t get(uint32_t rc) const noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : return _gp;
      case kX86RegClassMm : return _mm;
      case kX86RegClassK  : return _k;
      case kX86RegClassXyz: return _xyz;
    }

    return 0;
  }

  ASMJIT_INLINE uint32_t getGp() const noexcept { return get(kX86RegClassGp); }
  ASMJIT_INLINE uint32_t getMm() const noexcept { return get(kX86RegClassMm); }
  ASMJIT_INLINE uint32_t getK() const noexcept { return get(kX86RegClassK); }
  ASMJIT_INLINE uint32_t getXyz() const noexcept { return get(kX86RegClassXyz); }

  // --------------------------------------------------------------------------
  // [Zero]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void zero(uint32_t rc) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : _gp  = 0; break;
      case kX86RegClassMm : _mm  = 0; break;
      case kX86RegClassK  : _k   = 0; break;
      case kX86RegClassXyz: _xyz = 0; break;
    }
  }

  ASMJIT_INLINE void zeroGp() noexcept { zero(kX86RegClassGp); }
  ASMJIT_INLINE void zeroMm() noexcept { zero(kX86RegClassMm); }
  ASMJIT_INLINE void zeroK() noexcept { zero(kX86RegClassK); }
  ASMJIT_INLINE void zeroXyz() noexcept { zero(kX86RegClassXyz); }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void set(const X86RegMask& other) noexcept {
    _packed = other._packed;
  }

  ASMJIT_INLINE void set(uint32_t rc, uint32_t mask) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : _gp  = static_cast<uint16_t>(mask); break;
      case kX86RegClassMm : _mm  = static_cast<uint8_t >(mask); break;
      case kX86RegClassK  : _k   = static_cast<uint8_t >(mask); break;
      case kX86RegClassXyz: _xyz = static_cast<uint32_t>(mask); break;
    }
  }

  ASMJIT_INLINE void setGp(uint32_t mask) noexcept { return set(kX86RegClassGp, mask); }
  ASMJIT_INLINE void setMm(uint32_t mask) noexcept { return set(kX86RegClassMm, mask); }
  ASMJIT_INLINE void setK(uint32_t mask) noexcept { return set(kX86RegClassK, mask); }
  ASMJIT_INLINE void setXyz(uint32_t mask) noexcept { return set(kX86RegClassXyz, mask); }

  // --------------------------------------------------------------------------
  // [And]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void and_(const X86RegMask& other) noexcept {
    _packed.and_(other._packed);
  }

  ASMJIT_INLINE void and_(uint32_t rc, uint32_t mask) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : _gp  &= static_cast<uint16_t>(mask); break;
      case kX86RegClassMm : _mm  &= static_cast<uint8_t >(mask); break;
      case kX86RegClassK  : _k   &= static_cast<uint8_t >(mask); break;
      case kX86RegClassXyz: _xyz &= static_cast<uint32_t>(mask); break;
    }
  }

  ASMJIT_INLINE void andGp(uint32_t mask) noexcept { and_(kX86RegClassGp, mask); }
  ASMJIT_INLINE void andMm(uint32_t mask) noexcept { and_(kX86RegClassMm, mask); }
  ASMJIT_INLINE void andK(uint32_t mask) noexcept { and_(kX86RegClassK, mask); }
  ASMJIT_INLINE void andXyz(uint32_t mask) noexcept { and_(kX86RegClassXyz, mask); }

  // --------------------------------------------------------------------------
  // [AndNot]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void andNot(const X86RegMask& other) noexcept {
    _packed.andNot(other._packed);
  }

  ASMJIT_INLINE void andNot(uint32_t rc, uint32_t mask) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : _gp  &= ~static_cast<uint16_t>(mask); break;
      case kX86RegClassMm : _mm  &= ~static_cast<uint8_t >(mask); break;
      case kX86RegClassK  : _k   &= ~static_cast<uint8_t >(mask); break;
      case kX86RegClassXyz: _xyz &= ~static_cast<uint32_t>(mask); break;
    }
  }

  ASMJIT_INLINE void andNotGp(uint32_t mask) noexcept { andNot(kX86RegClassGp, mask); }
  ASMJIT_INLINE void andNotMm(uint32_t mask) noexcept { andNot(kX86RegClassMm, mask); }
  ASMJIT_INLINE void andNotK(uint32_t mask) noexcept { andNot(kX86RegClassK, mask); }
  ASMJIT_INLINE void andNotXyz(uint32_t mask) noexcept { andNot(kX86RegClassXyz, mask); }

  // --------------------------------------------------------------------------
  // [Or]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void or_(const X86RegMask& other) noexcept {
    _packed.or_(other._packed);
  }

  ASMJIT_INLINE void or_(uint32_t rc, uint32_t mask) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);
    switch (rc) {
      case kX86RegClassGp : _gp  |= static_cast<uint16_t>(mask); break;
      case kX86RegClassMm : _mm  |= static_cast<uint8_t >(mask); break;
      case kX86RegClassK  : _k   |= static_cast<uint8_t >(mask); break;
      case kX86RegClassXyz: _xyz |= static_cast<uint32_t>(mask); break;
    }
  }

  ASMJIT_INLINE void orGp(uint32_t mask) noexcept { return or_(kX86RegClassGp, mask); }
  ASMJIT_INLINE void orMm(uint32_t mask) noexcept { return or_(kX86RegClassMm, mask); }
  ASMJIT_INLINE void orK(uint32_t mask) noexcept { return or_(kX86RegClassK, mask); }
  ASMJIT_INLINE void orXyz(uint32_t mask) noexcept { return or_(kX86RegClassXyz, mask); }

  // --------------------------------------------------------------------------
  // [Xor]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE void xor_(const X86RegMask& other) noexcept {
    _packed.xor_(other._packed);
  }

  ASMJIT_INLINE void xor_(uint32_t rc, uint32_t mask) noexcept {
    ASMJIT_ASSERT(rc < _kX86RegClassManagedCount);

    switch (rc) {
      case kX86RegClassGp : _gp  ^= static_cast<uint16_t>(mask); break;
      case kX86RegClassMm : _mm  ^= static_cast<uint8_t >(mask); break;
      case kX86RegClassK  : _k   ^= static_cast<uint8_t >(mask); break;
      case kX86RegClassXyz: _xyz ^= static_cast<uint32_t>(mask); break;
    }
  }

  ASMJIT_INLINE void xorGp(uint32_t mask) noexcept { xor_(kX86RegClassGp, mask); }
  ASMJIT_INLINE void xorMm(uint32_t mask) noexcept { xor_(kX86RegClassMm, mask); }
  ASMJIT_INLINE void xorK(uint32_t mask) noexcept { xor_(kX86RegClassK, mask); }
  ASMJIT_INLINE void xorXyz(uint32_t mask) noexcept { xor_(kX86RegClassXyz, mask); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union {
    struct {
      //! GP registers mask (16 bits).
      uint16_t _gp;
      //! MMX registers mask (8 bits).
      uint8_t _mm;
      //! K registers mask (8 bits).
      uint8_t _k;
      //! XMM/YMM/ZMM registers mask (32 bits).
      uint32_t _xyz;
    };

    //! Packed masks.
    UInt64 _packed;
  };
};

// ============================================================================
// [asmjit::<Fake>Reg]
// ============================================================================

// This is only defined by `x86operand_regs.cpp` when exporting registers.
#if defined(ASMJIT_EXPORTS_X86_REGS)

// Remap all classes to POD structs so they can be statically initialized
// without calling a constructor. Compiler will store these in .DATA section.
//
// Kept in union to prevent LTO warnings.
class X86RipReg { public: union { Operand::VRegOp _vreg; }; };
class X86SegReg { public: union { Operand::VRegOp _vreg; }; };
class X86GpReg  { public: union { Operand::VRegOp _vreg; }; };
class X86FpReg  { public: union { Operand::VRegOp _vreg; }; };
class X86KReg   { public: union { Operand::VRegOp _vreg; }; };
class X86MmReg  { public: union { Operand::VRegOp _vreg; }; };
class X86XmmReg { public: union { Operand::VRegOp _vreg; }; };
class X86YmmReg { public: union { Operand::VRegOp _vreg; }; };
class X86ZmmReg { public: union { Operand::VRegOp _vreg; }; };

#else

// ============================================================================
// [asmjit::X86Reg]
// ============================================================================

//! X86/X86 register base class.
class X86Reg : public Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy X86 register.
  ASMJIT_INLINE X86Reg() noexcept : Reg() {}
  //! Create a reference to `other` X86 register.
  ASMJIT_INLINE X86Reg(const X86Reg& other) noexcept : Reg(other) {}
  //! Create a reference to `other` X86 register and change the index to `index`.
  ASMJIT_INLINE X86Reg(const X86Reg& other, uint32_t index) noexcept : Reg(other, index) {}
  //! Create a custom X86 register.
  ASMJIT_INLINE X86Reg(uint32_t type, uint32_t index, uint32_t size) noexcept : Reg(type, index, size) {}
  //! Create non-initialized X86 register.
  explicit ASMJIT_INLINE X86Reg(const _NoInit&) noexcept : Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86Reg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86Reg)

  //! Get whether the register is a GP register (any size).
  ASMJIT_INLINE bool isGp() const noexcept { return _vreg.type <= kX86RegTypeGpq; }
  //! Get whether the register is a GPB register (8-bit).
  ASMJIT_INLINE bool isGpb() const noexcept { return _vreg.type <= _kX86RegTypePatchedGpbHi; }
  //! Get whether the register is a low GPB register (8-bit).
  ASMJIT_INLINE bool isGpbLo() const noexcept { return _vreg.type == kX86RegTypeGpbLo; }
  //! Get whether the register is a high GPB register (8-bit).
  ASMJIT_INLINE bool isGpbHi() const noexcept { return _vreg.type == kX86RegTypeGpbHi; }
  //! Get whether the register is a GPW register (16-bit).
  ASMJIT_INLINE bool isGpw() const noexcept { return _vreg.type == kX86RegTypeGpw; }
  //! Get whether the register is a GPD register (32-bit).
  ASMJIT_INLINE bool isGpd() const noexcept { return _vreg.type == kX86RegTypeGpd; }
  //! Get whether the register is a GPQ register (64-bit).
  ASMJIT_INLINE bool isGpq() const noexcept { return _vreg.type == kX86RegTypeGpq; }

  //! Get whether the register is an FPU register (80-bit).
  ASMJIT_INLINE bool isFp() const noexcept { return _vreg.type == kX86RegTypeFp; }
  //! Get whether the register is an MMX register (64-bit).
  ASMJIT_INLINE bool isMm() const noexcept { return _vreg.type == kX86RegTypeMm; }

  //! Get whether the register is a K register (64-bit).
  ASMJIT_INLINE bool isK() const noexcept { return _vreg.type == kX86RegTypeK; }

  //! Get whether the register is an XMM register (128-bit).
  ASMJIT_INLINE bool isXmm() const noexcept { return _vreg.type == kX86RegTypeXmm; }
  //! Get whether the register is a YMM register (256-bit).
  ASMJIT_INLINE bool isYmm() const noexcept { return _vreg.type == kX86RegTypeYmm; }
  //! Get whether the register is a ZMM register (512-bit).
  ASMJIT_INLINE bool isZmm() const noexcept { return _vreg.type == kX86RegTypeZmm; }

  //! Get whether the register is RIP.
  ASMJIT_INLINE bool isRip() const noexcept { return _vreg.type == kX86RegTypeRip; }
  //! Get whether the register is a segment register.
  ASMJIT_INLINE bool isSeg() const noexcept { return _vreg.type == kX86RegTypeSeg; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! Get whether the `op` operand is low or high GPB register.
  static ASMJIT_INLINE bool isGpbReg(const Operand& op) noexcept {
    const uint32_t mask = Utils::pack32_2x8_1x16(
      0xFF, 0xFF, ~(_kX86RegTypePatchedGpbHi << 8) & 0xFF00);

    return (op._packed[0].u32[0] & mask) == Utils::pack32_2x8_1x16(kTypeReg, 1, 0x0000);
  }
};

// ============================================================================
// [asmjit::X86RipReg]
// ============================================================================

//! X86/X64 RIP register.
class X86RipReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a RIP register.
  ASMJIT_INLINE X86RipReg() noexcept : X86Reg(kX86RegTypeRip, 0, 0) {}
  //! Create a reference to `other` RIP register.
  ASMJIT_INLINE X86RipReg(const X86RipReg& other) noexcept : X86Reg(other) {}
  //! Create non-initialized RIP register.
  explicit ASMJIT_INLINE X86RipReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86RipReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86RipReg)
};

// ============================================================================
// [asmjit::X86SegReg]
// ============================================================================

//! X86/X64 segment register.
class X86SegReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy segment register.
  ASMJIT_INLINE X86SegReg() noexcept : X86Reg() {}
  //! Create a reference to `other` segment register.
  ASMJIT_INLINE X86SegReg(const X86SegReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` segment register and change the index to `index`.
  ASMJIT_INLINE X86SegReg(const X86SegReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom segment register.
  ASMJIT_INLINE X86SegReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized segment register.
  explicit ASMJIT_INLINE X86SegReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86SegReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86SegReg)
};

// ============================================================================
// [asmjit::X86GpReg]
// ============================================================================

//! X86/X64 general purpose register (GPB, GPW, GPD, GPQ).
class X86GpReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy Gp register.
  ASMJIT_INLINE X86GpReg() noexcept : X86Reg() {}
  //! Create a reference to `other` Gp register.
  ASMJIT_INLINE X86GpReg(const X86GpReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` Gp register and change the index to `index`.
  ASMJIT_INLINE X86GpReg(const X86GpReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom Gp register.
  ASMJIT_INLINE X86GpReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized Gp register.
  explicit ASMJIT_INLINE X86GpReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86GpReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86GpReg)

  // --------------------------------------------------------------------------
  // [X86GpReg Cast]
  // --------------------------------------------------------------------------

  //! Cast this register to the same register type/size as `other`.
  //!
  //! This function has been designed to help with maintaining code that runs
  //! in both 32-bit and 64-bit modes. If you have registers that have mixed
  //! types, use `X86GpReg::as()` to cast one type to another.
  ASMJIT_INLINE X86GpReg as(const X86GpReg& other) const noexcept {
    return X86GpReg(other.getRegType(), getRegIndex(), other.getSize());
  }

  //! Cast this register to 8-bit (LO) part.
  ASMJIT_INLINE X86GpReg r8() const noexcept { return X86GpReg(kX86RegTypeGpbLo, getRegIndex(), 1); }
  //! Cast this register to 8-bit (LO) part.
  ASMJIT_INLINE X86GpReg r8Lo() const noexcept { return X86GpReg(kX86RegTypeGpbLo, getRegIndex(), 1); }
  //! Cast this register to 8-bit (HI) part.
  ASMJIT_INLINE X86GpReg r8Hi() const noexcept { return X86GpReg(kX86RegTypeGpbHi, getRegIndex(), 1); }

  //! Cast this register to 16-bit.
  ASMJIT_INLINE X86GpReg r16() const noexcept { return X86GpReg(kX86RegTypeGpw, getRegIndex(), 2); }
  //! Cast this register to 32-bit.
  ASMJIT_INLINE X86GpReg r32() const noexcept { return X86GpReg(kX86RegTypeGpd, getRegIndex(), 4); }
  //! Cast this register to 64-bit.
  ASMJIT_INLINE X86GpReg r64() const noexcept { return X86GpReg(kX86RegTypeGpq, getRegIndex(), 8); }
};

// ============================================================================
// [asmjit::X86FpReg]
// ============================================================================

//! X86/X64 80-bit Fp register.
class X86FpReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy Fp register.
  ASMJIT_INLINE X86FpReg() noexcept : X86Reg() {}
  //! Create a reference to `other` Fp register.
  ASMJIT_INLINE X86FpReg(const X86FpReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` Fp register and change the index to `index`.
  ASMJIT_INLINE X86FpReg(const X86FpReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom Fp register.
  ASMJIT_INLINE X86FpReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized Fp register.
  explicit ASMJIT_INLINE X86FpReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86FpReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86FpReg)
};

// ============================================================================
// [asmjit::X86MmReg]
// ============================================================================

//! X86/X64 64-bit Mm register (MMX+).
//!
//! Structure of MMX register and it's memory mapping:
//!
//! ~~~
//!       Memory Bytes
//! +--+--+--+--+--+--+--+--+
//! |00|01|02|03|04|05|06|07|
//! +--+--+--+--+--+--+--+--+
//!
//!       MMX Register
//! +-----------------------+
//! |         QWORD         |
//! +-----------+-----------+
//! |  HI-DWORD |  LO-DWORD |
//! +-----------+-----------+
//! |  W3 |  W2 |  W1 |  W0 |
//! +--+--+--+--+--+--+--+--+
//! |07|06|05|04|03|02|01|00|
//! +--+--+--+--+--+--+--+--+
//! ~~~
//!
//! Move instruction semantics:
//!
//!   - `movd` - writes 4-bytes in low DWORD and clears high DWORD.
//!   - `movq` - writes 8-bytes in `QWORD`.
class X86MmReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy Mm register.
  ASMJIT_INLINE X86MmReg() noexcept : X86Reg() {}
  //! Create a reference to `other` Mm register.
  ASMJIT_INLINE X86MmReg(const X86MmReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` Mm register and change the index to `index`.
  ASMJIT_INLINE X86MmReg(const X86MmReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom Mm register.
  ASMJIT_INLINE X86MmReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized Mm register.
  explicit ASMJIT_INLINE X86MmReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86MmReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86MmReg)
};

// ============================================================================
// [asmjit::X86KReg]
// ============================================================================

//! X86/X64 64-bit K register (AVX512+).
class X86KReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy K register.
  ASMJIT_INLINE X86KReg() noexcept : X86Reg() {}
  //! Create a reference to `other` K register.
  ASMJIT_INLINE X86KReg(const X86KReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` K register and change the index to `index`.
  ASMJIT_INLINE X86KReg(const X86KReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom K register.
  ASMJIT_INLINE X86KReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized K register.
  explicit ASMJIT_INLINE X86KReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86KReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86KReg)
};

// ============================================================================
// [asmjit::X86XmmReg]
// ============================================================================

//! X86/X64 128-bit XMM register (SSE+).
//!
//! Structure of XMM register and it's memory mapping:
//!
//! ~~~
//!                   Memory Bytes
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//! |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//!
//!                   XMM Register
//! +-----------------------------------------------+
//! |                    DQWORD                     |
//! +-----------------------+-----------------------+
//! |      HI-QWORD/PD      |      LO-QWORD/SD      |
//! +-----------+-----------+-----------+-----------+
//! |   D3/PS   |   D2/PS   |   D1/PS   |   D0/SS   |
//! +-----------+-----------+-----------+-----------+
//! |  W7 |  W6 |  W5 |  W4 |  W3 |  W2 |  W1 |  W0 |
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//! |15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//! ~~~
//!
//! Move instruction semantics:
//!
//!   - `movd` - writes 4-bytes in `D0` and clears the rest.
//!   - `movq` - writes 8-bytes in low QWORD and clears the rest.
//!   - `movq2dq` - writes 8 bytes in low QWORD and clears the rest.
//!
//!   - `movss` - writes 4-bytes in `D0`
//!       (the rest is zeroed only if the source operand is a memory location).
//!   - `movsd` - writes 8-bytes in low QWORD
//!       (the rest is zeroed only if the source operand is a memory location).
//!
//!   - `movaps`,
//!     `movups`,
//!     `movapd`,
//!     `movupd`,
//!     `movdqu`,
//!     `movdqa`,
//!     `lddqu` - writes 16-bytes in DQWORD.
//!
//!   - `movlps`,
//!     `movlpd`,
//!     `movhlps` - writes 8-bytes in low QWORD and keeps the rest untouched.
//!
//!   - `movhps`,
//!     `movhpd`,
//!     `movlhps` - writes 8-bytes in high QWORD and keeps the rest untouched.
//!
//!   - `movddup`,
//!   - `movsldup`,
//!   - `movshdup` - writes 16 bytes in DQWORD.
class X86XmmReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy XMM register.
  ASMJIT_INLINE X86XmmReg() noexcept : X86Reg() {}
  //! Create a reference to `other` XMM register.
  ASMJIT_INLINE X86XmmReg(const X86XmmReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` XMM register and change the index to `index`.
  ASMJIT_INLINE X86XmmReg(const X86XmmReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom XMM register.
  ASMJIT_INLINE X86XmmReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized XMM register.
  explicit ASMJIT_INLINE X86XmmReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86XmmReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86XmmReg)

  // --------------------------------------------------------------------------
  // [X86XmmReg Cast]
  // --------------------------------------------------------------------------

  //! Cast this register to XMM (clone).
  ASMJIT_INLINE X86XmmReg xmm() const noexcept { return X86XmmReg(kX86RegTypeXmm, getRegIndex(), 16); }
  //! Cast this register to YMM.
  ASMJIT_INLINE X86YmmReg ymm() const noexcept;
  //! Cast this register to ZMM.
  ASMJIT_INLINE X86ZmmReg zmm() const noexcept;
};

// ============================================================================
// [asmjit::X86YmmReg]
// ============================================================================

//! X86/X64 256-bit YMM register (AVX+).
//!
//! Structure of YMM register and it's memory mapping:
//!
//! ~~~
//!                                           Memory Bytes
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//! |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//!
//!                                           YMM Register
//! +-----------------------------------------------+-----------------------------------------------+
//! |                  HI-DQWORD                    |                  LO-DQWORD                    |
//! +-----------------------+-----------------------+-----------------------+-----------------------+
//! |         Q3/PD         |         Q2/PD         |         Q1/PD         |         Q0/SD         |
//! +-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
//! |   D7/PS   |   D6/PS   |   D5/PS   |   D4/PS   |   D3/PS   |   D2/PS   |   D1/PS   |   D0/SS   |
//! +-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
//! | W15 | W14 | W13 | W12 | W11 | W10 |  W9 |  W8 |  W7 |  W6 |  W5 |  W4 |  W3 |  W2 |  W1 |  W0 |
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//! |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
//! +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//! ~~~
class X86YmmReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy YMM register.
  ASMJIT_INLINE X86YmmReg() noexcept : X86Reg() {}
  //! Create a reference to `other` YMM register.
  ASMJIT_INLINE X86YmmReg(const X86YmmReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` YMM register and change the index to `index`.
  ASMJIT_INLINE X86YmmReg(const X86YmmReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom YMM register.
  ASMJIT_INLINE X86YmmReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized YMM register.
  explicit ASMJIT_INLINE X86YmmReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86YmmReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86YmmReg)

  // --------------------------------------------------------------------------
  // [X86YmmReg Cast]
  // --------------------------------------------------------------------------

  //! Cast this register to XMM.
  ASMJIT_INLINE X86XmmReg xmm() const  noexcept{ return X86XmmReg(kX86RegTypeXmm, getRegIndex(), 16); }
  //! Cast this register to YMM (clone).
  ASMJIT_INLINE X86YmmReg ymm() const noexcept { return X86YmmReg(kX86RegTypeYmm, getRegIndex(), 32); }
  //! Cast this register to ZMM.
  ASMJIT_INLINE X86ZmmReg zmm() const noexcept;
};

ASMJIT_INLINE X86YmmReg X86XmmReg::ymm() const noexcept { return X86YmmReg(kX86RegTypeYmm, getRegIndex(), 32); }

// ============================================================================
// [asmjit::X86ZmmReg]
// ============================================================================

//! X86/X64 512-bit ZMM register (AVX512+).
class X86ZmmReg : public X86Reg {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a dummy ZMM register.
  ASMJIT_INLINE X86ZmmReg() noexcept : X86Reg() {}
  //! Create a reference to `other` ZMM register.
  ASMJIT_INLINE X86ZmmReg(const X86ZmmReg& other) noexcept : X86Reg(other) {}
  //! Create a reference to `other` ZMM register and change the index to `index`.
  ASMJIT_INLINE X86ZmmReg(const X86ZmmReg& other, uint32_t index) noexcept : X86Reg(other, index) {}
  //! Create a custom ZMM register.
  ASMJIT_INLINE X86ZmmReg(uint32_t type, uint32_t index, uint32_t size) noexcept : X86Reg(type, index, size) {}
  //! Create non-initialized ZMM register.
  explicit ASMJIT_INLINE X86ZmmReg(const _NoInit&) noexcept : X86Reg(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86ZmmReg Specific]
  // --------------------------------------------------------------------------

  ASMJIT_REG_OP(X86ZmmReg)

  // --------------------------------------------------------------------------
  // [X86ZmmReg Cast]
  // --------------------------------------------------------------------------

  //! Cast this register to XMM.
  ASMJIT_INLINE X86XmmReg xmm() const noexcept { return X86XmmReg(kX86RegTypeXmm, getRegIndex(), 16); }
  //! Cast this register to YMM.
  ASMJIT_INLINE X86YmmReg ymm() const noexcept { return X86YmmReg(kX86RegTypeYmm, getRegIndex(), 32); }
  //! Cast this register to ZMM (clone).
  ASMJIT_INLINE X86ZmmReg zmm() const noexcept { return X86ZmmReg(kX86RegTypeZmm, getRegIndex(), 64); }
};

ASMJIT_INLINE X86ZmmReg X86XmmReg::zmm() const noexcept { return X86ZmmReg(kX86RegTypeZmm, getRegIndex(), 64); }
ASMJIT_INLINE X86ZmmReg X86YmmReg::zmm() const noexcept { return X86ZmmReg(kX86RegTypeZmm, getRegIndex(), 64); }

// ============================================================================
// [asmjit::X86Mem]
// ============================================================================

//! X86 memory operand.
class X86Mem : public BaseMem {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86Mem() noexcept : BaseMem(NoInit) {
    reset();
  }

  ASMJIT_INLINE X86Mem(const Label& label, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeLabel, 0, label._base.id);
    _init_packed_d2_d3(kInvalidValue, disp);
  }

  ASMJIT_INLINE X86Mem(const Label& label, const X86GpReg& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeLabel,
      (kX86MemVSibGpz << kX86MemVSibIndex)
        + (shift << kX86MemShiftIndex),
      label.getId());
    _vmem.index = index.getRegIndex();
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const X86RipReg& rip, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_UNUSED(rip);
    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeRip, 0, kInvalidValue);
    _init_packed_d2_d3(kInvalidValue, disp);
  }

  ASMJIT_INLINE X86Mem(const X86GpReg& base, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(base)
        + (kX86MemVSibGpz << kX86MemVSibIndex),
      base.getRegIndex());
    _init_packed_d2_d3(kInvalidValue, disp);
  }

  ASMJIT_INLINE X86Mem(const X86GpReg& base, const X86GpReg& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(base) + (shift << kX86MemShiftIndex),
      base.getRegIndex());
    _vmem.index = index.getRegIndex();
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const X86GpReg& base, const X86XmmReg& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(base)
        + (kX86MemVSibXmm << kX86MemVSibIndex)
        + (shift << kX86MemShiftIndex),
      base.getRegIndex());
    _vmem.index = index.getRegIndex();
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const X86GpReg& base, const X86YmmReg& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(base)
        + (kX86MemVSibYmm << kX86MemVSibIndex)
        + (shift << kX86MemShiftIndex),
      base.getRegIndex());
    _vmem.index = index.getRegIndex();
    _vmem.displacement = disp;
  }

#if !defined(ASMJIT_DISABLE_COMPILER)
  ASMJIT_INLINE X86Mem(const Label& label, const X86GpVar& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeLabel,
      (kX86MemVSibGpz << kX86MemVSibIndex)
        + (shift << kX86MemShiftIndex),
      label.getId());
    _vmem.index = ASMJIT_OP_ID(index);
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const X86GpVar& base, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(reinterpret_cast<const Var&>(base))
        + (kX86MemVSibGpz << kX86MemVSibIndex),
      ASMJIT_OP_ID(base));
    _init_packed_d2_d3(kInvalidValue, disp);
  }

  ASMJIT_INLINE X86Mem(const X86GpVar& base, const X86GpVar& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(reinterpret_cast<const Var&>(base))
        + (shift << kX86MemShiftIndex),
      ASMJIT_OP_ID(base));
    _vmem.index = ASMJIT_OP_ID(index);
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const X86GpVar& base, const X86XmmVar& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(reinterpret_cast<const Var&>(base))
        + (kX86MemVSibXmm << kX86MemVSibIndex)
        + (shift << kX86MemShiftIndex),
      ASMJIT_OP_ID(base));
    _vmem.index = ASMJIT_OP_ID(index);
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const X86GpVar& base, const X86YmmVar& index, uint32_t shift, int32_t disp, uint32_t size = 0) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, kMemTypeBaseIndex,
      _getGpdFlags(reinterpret_cast<const Var&>(base))
        + (kX86MemVSibYmm << kX86MemVSibIndex)
        + (shift << kX86MemShiftIndex),
      ASMJIT_OP_ID(base));
    _vmem.index = ASMJIT_OP_ID(index);
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const _Init&, uint32_t memType, const X86Var& base, int32_t disp, uint32_t size) noexcept : BaseMem(NoInit) {
    _init_packed_op_sz_b0_b1_id(kTypeMem, size, memType, 0, ASMJIT_OP_ID(base));
    _vmem.index = kInvalidValue;
    _vmem.displacement = disp;
  }

  ASMJIT_INLINE X86Mem(const _Init&, uint32_t memType, const X86Var& base, const X86GpVar& index, uint32_t shift, int32_t disp, uint32_t size) noexcept : BaseMem(NoInit) {
    ASMJIT_ASSERT(shift <= 3);

    _init_packed_op_sz_b0_b1_id(kTypeMem, size, memType, shift << kX86MemShiftIndex, ASMJIT_OP_ID(base));
    _vmem.index = ASMJIT_OP_ID(index);
    _vmem.displacement = disp;
  }
#endif // !ASMJIT_DISABLE_COMPILER

  ASMJIT_INLINE X86Mem(const X86Mem& other) noexcept : BaseMem(other) {}
  explicit ASMJIT_INLINE X86Mem(const _NoInit&) noexcept : BaseMem(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86Mem Specific]
  // --------------------------------------------------------------------------

  //! Clone X86Mem operand.
  ASMJIT_INLINE X86Mem clone() const noexcept {
    return X86Mem(*this);
  }

  //! Reset X86Mem operand.
  ASMJIT_INLINE void reset() noexcept {
    _init_packed_op_sz_b0_b1_id(kTypeMem, 0, kMemTypeBaseIndex, 0, kInvalidValue);
    _init_packed_d2_d3(kInvalidValue, 0);
  }

  //! \internal
  ASMJIT_INLINE void _init(uint32_t memType, uint32_t base, int32_t disp, uint32_t size) noexcept {
    _init_packed_op_sz_b0_b1_id(kTypeMem, size, memType, 0, base);
    _vmem.index = kInvalidValue;
    _vmem.displacement = disp;
  }

  // --------------------------------------------------------------------------
  // [Segment]
  // --------------------------------------------------------------------------

  //! Get whether the memory operand has segment override prefix.
  ASMJIT_INLINE bool hasSegment() const noexcept {
    return (_vmem.flags & kX86MemSegMask) != (kX86SegDefault << kX86MemSegIndex);
  }

  //! Get memory operand segment, see `X86Seg`.
  ASMJIT_INLINE uint32_t getSegment() const noexcept{
    return (static_cast<uint32_t>(_vmem.flags) >> kX86MemSegIndex) & kX86MemSegBits;
  }

  //! Set memory operand segment, see `X86Seg`.
  ASMJIT_INLINE X86Mem& setSegment(uint32_t segIndex) noexcept {
    _vmem.flags = static_cast<uint8_t>(
      (static_cast<uint32_t>(_vmem.flags) & kX86MemSegMask) + (segIndex << kX86MemSegIndex));
    return *this;
  }

  //! Set memory operand segment, see `X86Seg`.
  ASMJIT_INLINE X86Mem& setSegment(const X86SegReg& seg) noexcept {
    return setSegment(seg.getRegIndex());
  }

  // --------------------------------------------------------------------------
  // [Gpd]
  // --------------------------------------------------------------------------

  //! Get whether the memory operand has 32-bit GP base.
  ASMJIT_INLINE bool hasGpdBase() const noexcept {
    return (_packed[0].u32[0] & Utils::pack32_4x8(0x00, 0x00, 0x00, kX86MemGpdMask)) != 0;
  }

  //! Set whether the memory operand has 32-bit GP base.
  ASMJIT_INLINE X86Mem& setGpdBase() noexcept {
    _packed[0].u32[0] |= Utils::pack32_4x8(0x00, 0x00, 0x00, kX86MemGpdMask);
    return *this;
  }

  //! Set whether the memory operand has 32-bit GP base to `b`.
  ASMJIT_INLINE X86Mem& setGpdBase(uint32_t b) noexcept {
    _packed[0].u32[0] &=~Utils::pack32_4x8(0x00, 0x00, 0x00, kX86MemGpdMask);
    _packed[0].u32[0] |= Utils::pack32_4x8(0x00, 0x00, 0x00, b << kX86MemGpdIndex);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [VSib]
  // --------------------------------------------------------------------------

  //! Get V-SIB type.
  ASMJIT_INLINE uint32_t getVSib() const noexcept {
    return (static_cast<uint32_t>(_vmem.flags) >> kX86MemVSibIndex) & kX86MemVSibBits;
  }

  //! Set V-SIB type.
  ASMJIT_INLINE X86Mem& _setVSib(uint32_t vsib) noexcept {
    _packed[0].u32[0] &=~Utils::pack32_4x8(0x00, 0x00, 0x00, kX86MemVSibMask);
    _packed[0].u32[0] |= Utils::pack32_4x8(0x00, 0x00, 0x00, vsib << kX86MemVSibIndex);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Size]
  // --------------------------------------------------------------------------

  //! Set memory operand size.
  ASMJIT_INLINE X86Mem& setSize(uint32_t size) noexcept {
    _vmem.size = static_cast<uint8_t>(size);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Base]
  // --------------------------------------------------------------------------

  //! Get whether the memory operand has base register.
  ASMJIT_INLINE bool hasBase() const noexcept {
    return _vmem.base != kInvalidValue;
  }

  //! Get memory operand base register code, variable id, or `kInvalidValue`.
  ASMJIT_INLINE uint32_t getBase() const noexcept {
    return _vmem.base;
  }

  //! Set memory operand base register code, variable id, or `kInvalidValue`.
  ASMJIT_INLINE X86Mem& setBase(uint32_t base) noexcept {
    _vmem.base = base;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Index]
  // --------------------------------------------------------------------------

  //! Get whether the memory operand has index.
  ASMJIT_INLINE bool hasIndex() const noexcept {
    return _vmem.index != kInvalidValue;
  }

  //! Get memory operand index register code, variable id, or `kInvalidValue`.
  ASMJIT_INLINE uint32_t getIndex() const noexcept {
    return _vmem.index;
  }

  //! Set memory operand index register code, variable id, or `kInvalidValue`.
  ASMJIT_INLINE X86Mem& setIndex(uint32_t index) noexcept {
    _vmem.index = index;
    return *this;
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86GpReg& index) noexcept {
    _vmem.index = index.getRegIndex();
    return _setVSib(kX86MemVSibGpz);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86GpReg& index, uint32_t shift) noexcept {
    _vmem.index = index.getRegIndex();
    return _setVSib(kX86MemVSibGpz).setShift(shift);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86XmmReg& index) noexcept {
    _vmem.index = index.getRegIndex();
    return _setVSib(kX86MemVSibXmm);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86XmmReg& index, uint32_t shift) noexcept {
    _vmem.index = index.getRegIndex();
    return _setVSib(kX86MemVSibXmm).setShift(shift);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86YmmReg& index) noexcept {
    _vmem.index = index.getRegIndex();
    return _setVSib(kX86MemVSibYmm);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86YmmReg& index, uint32_t shift) noexcept {
    _vmem.index = index.getRegIndex();
    return _setVSib(kX86MemVSibYmm).setShift(shift);
  }

#if !defined(ASMJIT_DISABLE_COMPILER)
  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86GpVar& index) noexcept {
    _vmem.index = ASMJIT_OP_ID(index);
    return _setVSib(kX86MemVSibGpz);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86GpVar& index, uint32_t shift) noexcept {
    _vmem.index = ASMJIT_OP_ID(index);
    return _setVSib(kX86MemVSibGpz).setShift(shift);
  }


  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86XmmVar& index) noexcept {
    _vmem.index = ASMJIT_OP_ID(index);
    return _setVSib(kX86MemVSibXmm);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86XmmVar& index, uint32_t shift) noexcept {
    _vmem.index = ASMJIT_OP_ID(index);
    return _setVSib(kX86MemVSibXmm).setShift(shift);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86YmmVar& index) noexcept {
    _vmem.index = ASMJIT_OP_ID(index);
    return _setVSib(kX86MemVSibYmm);
  }

  //! Set memory index.
  ASMJIT_INLINE X86Mem& setIndex(const X86YmmVar& index, uint32_t shift) noexcept {
    _vmem.index = ASMJIT_OP_ID(index);
    return _setVSib(kX86MemVSibYmm).setShift(shift);
  }
#endif // !ASMJIT_DISABLE_COMPILER

  //! Reset memory index.
  ASMJIT_INLINE X86Mem& resetIndex() noexcept {
    _vmem.index = kInvalidValue;
    return _setVSib(kX86MemVSibGpz);
  }

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  //! Get whether the memory operand has base and index register.
  ASMJIT_INLINE bool hasBaseOrIndex() const noexcept {
    return _vmem.base != kInvalidValue || _vmem.index != kInvalidValue;
  }

  //! Get whether the memory operand has base and index register.
  ASMJIT_INLINE bool hasBaseAndIndex() const noexcept {
    return _vmem.base != kInvalidValue && _vmem.index != kInvalidValue;
  }

  // --------------------------------------------------------------------------
  // [Shift]
  // --------------------------------------------------------------------------

  //! Get whether the memory operand has shift used.
  ASMJIT_INLINE bool hasShift() const noexcept {
    return (_vmem.flags & kX86MemShiftMask) != 0;
  }

  //! Get memory operand index shift (0, 1, 2 or 3).
  ASMJIT_INLINE uint32_t getShift() const noexcept {
    return _vmem.flags >> kX86MemShiftIndex;
  }

  //! Set memory operand index shift (0, 1, 2 or 3).
  ASMJIT_INLINE X86Mem& setShift(uint32_t shift) noexcept {
    _packed[0].u32[0] &=~Utils::pack32_4x8(0x00, 0x00, 0x00, kX86MemShiftMask);
    _packed[0].u32[0] |= Utils::pack32_4x8(0x00, 0x00, 0x00, shift << kX86MemShiftIndex);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Displacement]
  // --------------------------------------------------------------------------

  //! Get memory operand relative displacement.
  ASMJIT_INLINE int32_t getDisplacement() const noexcept {
    return _vmem.displacement;
  }

  //! Set memory operand relative displacement.
  ASMJIT_INLINE X86Mem& setDisplacement(int32_t disp) noexcept {
    _vmem.displacement = disp;
    return *this;
  }

  //! Reset memory operand relative displacement.
  ASMJIT_INLINE X86Mem& resetDisplacement() noexcept {
    _vmem.displacement = 0;
    return *this;
  }

  //! Adjust memory operand relative displacement by `disp`.
  ASMJIT_INLINE X86Mem& adjust(int32_t disp) noexcept {
    _vmem.displacement += disp;
    return *this;
  }

  //! Get new memory operand adjusted by `disp`.
  ASMJIT_INLINE X86Mem adjusted(int32_t disp) const noexcept {
    X86Mem result(*this);
    result.adjust(disp);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86Mem& operator=(const X86Mem& other) noexcept {
    _copy(other);
    return *this;
  }

  ASMJIT_INLINE bool operator==(const X86Mem& other) const noexcept {
    return (_packed[0] == other._packed[0]) && (_packed[1] == other._packed[1]) ;
  }

  ASMJIT_INLINE bool operator!=(const X86Mem& other) const noexcept {
    return !(*this == other);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static ASMJIT_INLINE uint32_t _getGpdFlags(const Operand& base) noexcept {
    return (base._vreg.size & 0x4) << (kX86MemGpdIndex - 2);
  }
};

// ============================================================================
// [asmjit::X86Var]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! Base class for all X86 variables.
class X86Var : public Var {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a new uninitialized `X86Var` instance.
  ASMJIT_INLINE X86Var() noexcept : Var(NoInit) { reset(); }
  //! Create a clone of `other`.
  ASMJIT_INLINE X86Var(const X86Var& other) noexcept : Var(other) {}
  //! Create a new uninitialized `X86Var` instance (internal).
  explicit ASMJIT_INLINE X86Var(const _NoInit&) noexcept : Var(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86Var Specific]
  // --------------------------------------------------------------------------

  //! Clone X86Var operand.
  ASMJIT_INLINE X86Var clone() const noexcept { return X86Var(*this); }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! Get register type.
  ASMJIT_INLINE uint32_t getRegType() const noexcept { return _vreg.type; }
  //! Get variable type.
  ASMJIT_INLINE uint32_t getVarType() const noexcept { return _vreg.vType; }

  //! Get whether the variable is a general purpose register.
  ASMJIT_INLINE bool isGp() const noexcept { return _vreg.type <= kX86RegTypeGpq; }
  //! Get whether the variable is a GPB (8-bit) register.
  ASMJIT_INLINE bool isGpb() const noexcept { return _vreg.type <= kX86RegTypeGpbHi; }
  //! Get whether the variable is a low GPB (8-bit) register.
  ASMJIT_INLINE bool isGpbLo() const noexcept { return _vreg.type == kX86RegTypeGpbLo; }
  //! Get whether the variable is a high GPB (8-bit) register.
  ASMJIT_INLINE bool isGpbHi() const noexcept { return _vreg.type == kX86RegTypeGpbHi; }
  //! Get whether the variable is a GPW (16-bit) register.
  ASMJIT_INLINE bool isGpw() const noexcept { return _vreg.type == kX86RegTypeGpw; }
  //! Get whether the variable is a GPD (32-bit) register.
  ASMJIT_INLINE bool isGpd() const noexcept { return _vreg.type == kX86RegTypeGpd; }
  //! Get whether the variable is a GPQ (64-bit) register.
  ASMJIT_INLINE bool isGpq() const noexcept { return _vreg.type == kX86RegTypeGpq; }

  //! Get whether the variable is MMX (64-bit) register.
  ASMJIT_INLINE bool isMm() const noexcept { return _vreg.type == kX86RegTypeMm; }
  //! Get whether the variable is K (64-bit) register.
  ASMJIT_INLINE bool isK() const noexcept { return _vreg.type == kX86RegTypeK; }

  //! Get whether the variable is XMM (128-bit) register.
  ASMJIT_INLINE bool isXmm() const noexcept { return _vreg.type == kX86RegTypeXmm; }
  //! Get whether the variable is YMM (256-bit) register.
  ASMJIT_INLINE bool isYmm() const noexcept { return _vreg.type == kX86RegTypeYmm; }
  //! Get whether the variable is ZMM (512-bit) register.
  ASMJIT_INLINE bool isZmm() const noexcept { return _vreg.type == kX86RegTypeZmm; }

  // --------------------------------------------------------------------------
  // [Memory Cast]
  // --------------------------------------------------------------------------

  //! Cast this variable to a memory operand.
  //!
  //! NOTE: Size of operand depends on native variable type, you can use other
  //! variants if you want specific one.
  ASMJIT_INLINE X86Mem m(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, getSize());
  }

  //! \overload
  ASMJIT_INLINE X86Mem m(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, getSize());
  }

  //! Cast this variable to 8-bit memory operand.
  ASMJIT_INLINE X86Mem m8(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 1);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m8(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 1);
  }

  //! Cast this variable to 16-bit memory operand.
  ASMJIT_INLINE X86Mem m16(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 2);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m16(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 2);
  }

  //! Cast this variable to 32-bit memory operand.
  ASMJIT_INLINE X86Mem m32(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 4);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m32(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 4);
  }

  //! Cast this variable to 64-bit memory operand.
  ASMJIT_INLINE X86Mem m64(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 8);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m64(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 8);
  }

  //! Cast this variable to 80-bit memory operand (long double).
  ASMJIT_INLINE X86Mem m80(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 10);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m80(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 10);
  }

  //! Cast this variable to 128-bit memory operand.
  ASMJIT_INLINE X86Mem m128(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 16);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m128(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 16);
  }

  //! Cast this variable to 256-bit memory operand.
  ASMJIT_INLINE X86Mem m256(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 32);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m256(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 32);
  }

  //! Cast this variable to 256-bit memory operand.
  ASMJIT_INLINE X86Mem m512(int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, disp, 64);
  }

  //! \overload
  ASMJIT_INLINE X86Mem m512(const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) const noexcept {
    return X86Mem(Init, kMemTypeStackIndex, *this, index, shift, disp, 64);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86Var& operator=(const X86Var& other) noexcept {
    _copy(other);
    return *this;
  }

  ASMJIT_INLINE bool operator==(const X86Var& other) const noexcept {
    return _packed[0] == other._packed[0];
  }

  ASMJIT_INLINE bool operator!=(const X86Var& other) const noexcept {
    return _packed[0] != other._packed[0];
  }

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

protected:
  ASMJIT_INLINE X86Var(const X86Var& other, uint32_t reg, uint32_t size) noexcept : Var(NoInit) {
    _init_packed_op_sz_w0_id(kTypeVar, size, (reg << 8) + other._vreg.index, other._base.id);
    _vreg.vType = other._vreg.vType;
  }
};
#endif // !ASMJIT_DISABLE_COMPILER

// ============================================================================
// [asmjit::X86GpVar]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! Gp variable.
class X86GpVar : public X86Var {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

protected:
  ASMJIT_INLINE X86GpVar(const X86GpVar& other, uint32_t reg, uint32_t size) noexcept
    : X86Var(other, reg, size) {}

public:
  //! Create a new uninitialized `X86GpVar` instance.
  ASMJIT_INLINE X86GpVar() noexcept : X86Var() {}
  //! Create a clone of `other`.
  ASMJIT_INLINE X86GpVar(const X86GpVar& other) noexcept : X86Var(other) {}
  //! Create a new uninitialized `X86GpVar` instance (internal).
  explicit ASMJIT_INLINE X86GpVar(const _NoInit&) noexcept : X86Var(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86GpVar Specific]
  // --------------------------------------------------------------------------

  //! Clone X86GpVar operand.
  ASMJIT_INLINE X86GpVar clone() const noexcept { return X86GpVar(*this); }
  //! Reset X86GpVar operand.
  ASMJIT_INLINE void reset() noexcept { X86Var::reset(); }

  // --------------------------------------------------------------------------
  // [X86GpVar Cast]
  // --------------------------------------------------------------------------

  //! Cast this variable to the same register type/size as `other`.
  //!
  //! This function has been designed to help with maintaining code that runs
  //! in both 32-bit and 64-bit modes. If you have variables that have mixed
  //! types, use `X86GpVar::as()` to cast one type to another.
  ASMJIT_INLINE X86GpVar as(const X86GpVar& other) const noexcept {
    return X86GpVar(*this, other.getRegType(), other.getSize());
  }

  //! Cast this variable to 8-bit (LO) part of variable.
  ASMJIT_INLINE X86GpVar r8() const noexcept { return X86GpVar(*this, kX86RegTypeGpbLo, 1); }
  //! Cast this variable to 8-bit (LO) part of variable.
  ASMJIT_INLINE X86GpVar r8Lo() const noexcept { return X86GpVar(*this, kX86RegTypeGpbLo, 1); }
  //! Cast this variable to 8-bit (HI) part of variable.
  ASMJIT_INLINE X86GpVar r8Hi() const noexcept { return X86GpVar(*this, kX86RegTypeGpbHi, 1); }

  //! Cast this variable to 16-bit part of variable.
  ASMJIT_INLINE X86GpVar r16() const noexcept { return X86GpVar(*this, kX86RegTypeGpw, 2); }
  //! Cast this variable to 32-bit part of variable.
  ASMJIT_INLINE X86GpVar r32() const noexcept { return X86GpVar(*this, kX86RegTypeGpd, 4); }
  //! Cast this variable to 64-bit part of variable.
  ASMJIT_INLINE X86GpVar r64() const noexcept { return X86GpVar(*this, kX86RegTypeGpq, 8); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86GpVar& operator=(const X86GpVar& other) noexcept { _copy(other); return *this; }

  ASMJIT_INLINE bool operator==(const X86GpVar& other) const noexcept { return X86Var::operator==(other); }
  ASMJIT_INLINE bool operator!=(const X86GpVar& other) const noexcept { return X86Var::operator!=(other); }
};
#endif // !ASMJIT_DISABLE_COMPILER

// ============================================================================
// [asmjit::X86MmVar]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! Mm variable.
class X86MmVar : public X86Var {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! Create a new uninitialized `X86MmVar` instance.
  ASMJIT_INLINE X86MmVar() noexcept : X86Var() {}
  //! Create a clone of `other`.
  ASMJIT_INLINE X86MmVar(const X86MmVar& other) noexcept : X86Var(other) {}

  //! Create a new uninitialized `X86MmVar` instance (internal).
  explicit ASMJIT_INLINE X86MmVar(const _NoInit&) noexcept : X86Var(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86MmVar Specific]
  // --------------------------------------------------------------------------

  //! Clone X86MmVar operand.
  ASMJIT_INLINE X86MmVar clone() const noexcept { return X86MmVar(*this); }
  //! Reset X86MmVar operand.
  ASMJIT_INLINE void reset() noexcept { X86Var::reset(); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86MmVar& operator=(const X86MmVar& other) noexcept { _copy(other); return *this; }

  ASMJIT_INLINE bool operator==(const X86MmVar& other) const noexcept { return X86Var::operator==(other); }
  ASMJIT_INLINE bool operator!=(const X86MmVar& other) const noexcept { return X86Var::operator!=(other); }
};
#endif // !ASMJIT_DISABLE_COMPILER

// ============================================================================
// [asmjit::X86XmmVar]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! XMM variable.
class X86XmmVar : public X86Var {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

protected:
  ASMJIT_INLINE X86XmmVar(const X86Var& other, uint32_t reg, uint32_t size) noexcept
    : X86Var(other, reg, size) {}

  friend class X86YmmVar;
  friend class X86ZmmVar;

public:
  //! Create a new uninitialized `X86XmmVar` instance.
  ASMJIT_INLINE X86XmmVar() noexcept : X86Var() {}
  //! Create a clone of `other`.
  ASMJIT_INLINE X86XmmVar(const X86XmmVar& other) noexcept : X86Var(other) {}
  //! Create a new uninitialized `X86XmmVar` instance (internal).
  explicit ASMJIT_INLINE X86XmmVar(const _NoInit&) noexcept : X86Var(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86XmmVar Specific]
  // --------------------------------------------------------------------------

  //! Clone X86XmmVar operand.
  ASMJIT_INLINE X86XmmVar clone() const noexcept { return X86XmmVar(*this); }
  //! Reset X86XmmVar operand.
  ASMJIT_INLINE void reset() noexcept { X86Var::reset(); }

  // --------------------------------------------------------------------------
  // [X86XmmVar Cast]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86XmmVar xmm() const noexcept { return X86XmmVar(*this); }
  ASMJIT_INLINE X86YmmVar ymm() const noexcept;
  ASMJIT_INLINE X86ZmmVar zmm() const noexcept;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86XmmVar& operator=(const X86XmmVar& other) noexcept { _copy(other); return *this; }

  ASMJIT_INLINE bool operator==(const X86XmmVar& other) const noexcept { return X86Var::operator==(other); }
  ASMJIT_INLINE bool operator!=(const X86XmmVar& other) const noexcept { return X86Var::operator!=(other); }
};
#endif // !ASMJIT_DISABLE_COMPILER

// ============================================================================
// [asmjit::X86YmmVar]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! YMM variable.
class  X86YmmVar : public X86Var {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

protected:
  ASMJIT_INLINE X86YmmVar(const X86Var& other, uint32_t reg, uint32_t size) noexcept
    : X86Var(other, reg, size) {}

  friend class X86XmmVar;
  friend class X86ZmmVar;

public:
  //! Create a new uninitialized `X86YmmVar` instance.
  ASMJIT_INLINE X86YmmVar() noexcept : X86Var() {}
  //! Create a clone of `other`.
  ASMJIT_INLINE X86YmmVar(const X86YmmVar& other) noexcept : X86Var(other) {}
  //! Create a new uninitialized `X86YmmVar` instance (internal).
  explicit ASMJIT_INLINE X86YmmVar(const _NoInit&) noexcept : X86Var(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86YmmVar Specific]
  // --------------------------------------------------------------------------

  //! Clone X86YmmVar operand.
  ASMJIT_INLINE X86YmmVar clone() const noexcept { return X86YmmVar(*this); }
  //! Reset X86YmmVar operand.
  ASMJIT_INLINE void reset() noexcept { X86Var::reset(); }

  // --------------------------------------------------------------------------
  // [X86YmmVar Cast]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86XmmVar xmm() const noexcept { return X86XmmVar(*this, kX86RegTypeXmm, 8); }
  ASMJIT_INLINE X86YmmVar ymm() const noexcept { return X86YmmVar(*this); }
  ASMJIT_INLINE X86ZmmVar zmm() const noexcept;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86YmmVar& operator=(const X86YmmVar& other) noexcept { _copy(other); return *this; }

  ASMJIT_INLINE bool operator==(const X86YmmVar& other) const noexcept { return X86Var::operator==(other); }
  ASMJIT_INLINE bool operator!=(const X86YmmVar& other) const noexcept { return X86Var::operator!=(other); }
};

ASMJIT_INLINE X86YmmVar X86XmmVar::ymm() const noexcept { return X86YmmVar(*this, kX86RegTypeYmm, 16); }
#endif // !ASMJIT_DISABLE_COMPILER

// ============================================================================
// [asmjit::X86ZmmVar]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! ZMM variable.
class X86ZmmVar : public X86Var {
public:
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

protected:
  ASMJIT_INLINE X86ZmmVar(const X86Var& other, uint32_t reg, uint32_t size) noexcept
    : X86Var(other, reg, size) {}

  friend class X86XmmVar;
  friend class X86YmmVar;

public:
  //! Create a new uninitialized `X86ZmmVar` instance.
  ASMJIT_INLINE X86ZmmVar() noexcept : X86Var() {}
  //! Create a clone of `other`.
  ASMJIT_INLINE X86ZmmVar(const X86ZmmVar& other) noexcept : X86Var(other) {}
  //! Create a new uninitialized `X86ZmmVar` instance (internal).
  explicit ASMJIT_INLINE X86ZmmVar(const _NoInit&) noexcept : X86Var(NoInit) {}

  // --------------------------------------------------------------------------
  // [X86ZmmVar Specific]
  // --------------------------------------------------------------------------

  //! Clone X86ZmmVar operand.
  ASMJIT_INLINE X86ZmmVar clone() const noexcept { return X86ZmmVar(*this); }
  //! Reset X86ZmmVar operand.
  ASMJIT_INLINE void reset() noexcept { X86Var::reset(); }

  // --------------------------------------------------------------------------
  // [X86ZmmVar Cast]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86XmmVar xmm() const noexcept { return X86XmmVar(*this, kX86RegTypeYmm, 8); }
  ASMJIT_INLINE X86YmmVar ymm() const noexcept { return X86YmmVar(*this, kX86RegTypeYmm, 16); }
  ASMJIT_INLINE X86ZmmVar zmm() const noexcept { return X86ZmmVar(*this); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ASMJIT_INLINE X86ZmmVar& operator=(const X86ZmmVar& other) noexcept { _copy(other); return *this; }

  ASMJIT_INLINE bool operator==(const X86ZmmVar& other) const noexcept { return X86Var::operator==(other); }
  ASMJIT_INLINE bool operator!=(const X86ZmmVar& other) const noexcept { return X86Var::operator!=(other); }
};

ASMJIT_INLINE X86ZmmVar X86XmmVar::zmm() const noexcept { return X86ZmmVar(*this, kX86RegTypeZmm, 32); }
ASMJIT_INLINE X86ZmmVar X86YmmVar::zmm() const noexcept { return X86ZmmVar(*this, kX86RegTypeZmm, 32); }
#endif // !ASMJIT_DISABLE_COMPILER
#endif

// ============================================================================
// [asmjit::X86RegData]
// ============================================================================

struct X86RegData {
  X86GpReg gpd[16];
  X86GpReg gpq[16];

  X86GpReg gpbLo[16];
  X86GpReg gpbHi[4];
  X86GpReg gpw[16];

  X86XmmReg xmm[32];
  X86YmmReg ymm[32];
  X86ZmmReg zmm[32];

  X86KReg k[8];
  X86FpReg fp[8];
  X86MmReg mm[8];

  X86SegReg seg[7];

  X86GpReg noGp;
  X86RipReg rip;
};
ASMJIT_VARAPI const X86RegData x86RegData;

// ============================================================================
// [asmjit::x86]
// ============================================================================

// The macro is only defined by `x86operand_regs.cpp` when exporting registers.
#if !defined(ASMJIT_EXPORTS_X86_REGS)

namespace x86 {

// ============================================================================
// [asmjit::x86 - Reg]
// ============================================================================

#define ASMJIT_DEF_REG(_Type_, _Name_, _Field_) \
  static const _Type_& _Name_ = x86RegData._Field_;

ASMJIT_DEF_REG(X86GpReg , eax  , gpd[0])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , ecx  , gpd[1])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , edx  , gpd[2])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , ebx  , gpd[3])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , esp  , gpd[4])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , ebp  , gpd[5])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , esi  , gpd[6])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , edi  , gpd[7])   //!< 32-bit GPD register.
ASMJIT_DEF_REG(X86GpReg , r8d  , gpd[8])   //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r9d  , gpd[9])   //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r10d , gpd[10])  //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r11d , gpd[11])  //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r12d , gpd[12])  //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r13d , gpd[13])  //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r14d , gpd[14])  //!< 32-bit GPD register (X64).
ASMJIT_DEF_REG(X86GpReg , r15d , gpd[15])  //!< 32-bit GPD register (X64).

ASMJIT_DEF_REG(X86GpReg , rax  , gpq[0])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rcx  , gpq[1])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rdx  , gpq[2])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rbx  , gpq[3])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rsp  , gpq[4])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rbp  , gpq[5])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rsi  , gpq[6])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , rdi  , gpq[7])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r8   , gpq[8])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r9   , gpq[9])   //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r10  , gpq[10])  //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r11  , gpq[11])  //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r12  , gpq[12])  //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r13  , gpq[13])  //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r14  , gpq[14])  //!< 64-bit GPQ register (X64).
ASMJIT_DEF_REG(X86GpReg , r15  , gpq[15])  //!< 64-bit GPQ register (X64).

ASMJIT_DEF_REG(X86GpReg , al   , gpbLo[0]) //!< 8-bit low GPB register.
ASMJIT_DEF_REG(X86GpReg , cl   , gpbLo[1]) //!< 8-bit low GPB register.
ASMJIT_DEF_REG(X86GpReg , dl   , gpbLo[2]) //!< 8-bit low GPB register.
ASMJIT_DEF_REG(X86GpReg , bl   , gpbLo[3]) //!< 8-bit low GPB register.
ASMJIT_DEF_REG(X86GpReg , spl  , gpbLo[4]) //!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , bpl  , gpbLo[5]) //!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , sil  , gpbLo[6]) //!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , dil  , gpbLo[7]) //!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r8b  , gpbLo[8]) //!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r9b  , gpbLo[9]) //!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r10b , gpbLo[10])//!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r11b , gpbLo[11])//!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r12b , gpbLo[12])//!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r13b , gpbLo[13])//!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r14b , gpbLo[14])//!< 8-bit low GPB register (X64).
ASMJIT_DEF_REG(X86GpReg , r15b , gpbLo[15])//!< 8-bit low GPB register (X64).

ASMJIT_DEF_REG(X86GpReg , ah   , gpbHi[0]) //!< 8-bit high GPB register.
ASMJIT_DEF_REG(X86GpReg , ch   , gpbHi[1]) //!< 8-bit high GPB register.
ASMJIT_DEF_REG(X86GpReg , dh   , gpbHi[2]) //!< 8-bit high GPB register.
ASMJIT_DEF_REG(X86GpReg , bh   , gpbHi[3]) //!< 8-bit high GPB register.

ASMJIT_DEF_REG(X86GpReg , ax   , gpw[0])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , cx   , gpw[1])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , dx   , gpw[2])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , bx   , gpw[3])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , sp   , gpw[4])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , bp   , gpw[5])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , si   , gpw[6])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , di   , gpw[7])   //!< 16-bit GPW register.
ASMJIT_DEF_REG(X86GpReg , r8w  , gpw[8])   //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r9w  , gpw[9])   //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r10w , gpw[10])  //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r11w , gpw[11])  //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r12w , gpw[12])  //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r13w , gpw[13])  //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r14w , gpw[14])  //!< 16-bit GPW register (X64).
ASMJIT_DEF_REG(X86GpReg , r15w , gpw[15])  //!< 16-bit GPW register (X64).

ASMJIT_DEF_REG(X86XmmReg, xmm0 , xmm[0])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm1 , xmm[1])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm2 , xmm[2])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm3 , xmm[3])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm4 , xmm[4])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm5 , xmm[5])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm6 , xmm[6])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm7 , xmm[7])   //!< 128-bit XMM register.
ASMJIT_DEF_REG(X86XmmReg, xmm8 , xmm[8])   //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm9 , xmm[9])   //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm10, xmm[10])  //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm11, xmm[11])  //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm12, xmm[12])  //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm13, xmm[13])  //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm14, xmm[14])  //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm15, xmm[15])  //!< 128-bit XMM register (X64).
ASMJIT_DEF_REG(X86XmmReg, xmm16, xmm[16])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm17, xmm[17])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm18, xmm[18])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm19, xmm[19])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm20, xmm[20])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm21, xmm[21])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm22, xmm[22])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm23, xmm[23])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm24, xmm[24])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm25, xmm[25])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm26, xmm[26])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm27, xmm[27])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm28, xmm[28])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm29, xmm[29])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm30, xmm[30])  //!< 128-bit XMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86XmmReg, xmm31, xmm[31])  //!< 128-bit XMM register (X64 & AVX512VL+).

ASMJIT_DEF_REG(X86YmmReg, ymm0 , ymm[0])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm1 , ymm[1])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm2 , ymm[2])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm3 , ymm[3])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm4 , ymm[4])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm5 , ymm[5])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm6 , ymm[6])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm7 , ymm[7])   //!< 256-bit YMM register.
ASMJIT_DEF_REG(X86YmmReg, ymm8 , ymm[8])   //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm9 , ymm[9])   //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm10, ymm[10])  //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm11, ymm[11])  //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm12, ymm[12])  //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm13, ymm[13])  //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm14, ymm[14])  //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm15, ymm[15])  //!< 256-bit YMM register (X64).
ASMJIT_DEF_REG(X86YmmReg, ymm16, ymm[16])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm17, ymm[17])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm18, ymm[18])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm19, ymm[19])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm20, ymm[20])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm21, ymm[21])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm22, ymm[22])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm23, ymm[23])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm24, ymm[24])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm25, ymm[25])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm26, ymm[26])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm27, ymm[27])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm28, ymm[28])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm29, ymm[29])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm30, ymm[30])  //!< 256-bit YMM register (X64 & AVX512VL+).
ASMJIT_DEF_REG(X86YmmReg, ymm31, ymm[31])  //!< 256-bit YMM register (X64 & AVX512VL+).

ASMJIT_DEF_REG(X86ZmmReg, zmm0 , zmm[0])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm1 , zmm[1])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm2 , zmm[2])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm3 , zmm[3])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm4 , zmm[4])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm5 , zmm[5])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm6 , zmm[6])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm7 , zmm[7])   //!< 512-bit ZMM register.
ASMJIT_DEF_REG(X86ZmmReg, zmm8 , zmm[8])   //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm9 , zmm[9])   //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm10, zmm[10])  //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm11, zmm[11])  //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm12, zmm[12])  //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm13, zmm[13])  //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm14, zmm[14])  //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm15, zmm[15])  //!< 512-bit ZMM register (X64).
ASMJIT_DEF_REG(X86ZmmReg, zmm16, zmm[16])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm17, zmm[17])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm18, zmm[18])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm19, zmm[19])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm20, zmm[20])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm21, zmm[21])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm22, zmm[22])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm23, zmm[23])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm24, zmm[24])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm25, zmm[25])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm26, zmm[26])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm27, zmm[27])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm28, zmm[28])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm29, zmm[29])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm30, zmm[30])  //!< 512-bit ZMM register (X64 & AVX512+).
ASMJIT_DEF_REG(X86ZmmReg, zmm31, zmm[31])  //!< 512-bit ZMM register (X64 & AVX512+).

ASMJIT_DEF_REG(X86KReg  , k0   , k[0])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k1   , k[1])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k2   , k[2])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k3   , k[3])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k4   , k[4])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k5   , k[5])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k6   , k[6])     //!< 64-bit K register.
ASMJIT_DEF_REG(X86KReg  , k7   , k[7])     //!< 64-bit K register.

ASMJIT_DEF_REG(X86FpReg , fp0  , fp[0])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp1  , fp[1])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp2  , fp[2])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp3  , fp[3])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp4  , fp[4])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp5  , fp[5])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp6  , fp[6])    //!< 80-bit FPU register.
ASMJIT_DEF_REG(X86FpReg , fp7  , fp[7])    //!< 80-bit FPU register.

ASMJIT_DEF_REG(X86MmReg , mm0  , mm[0])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm1  , mm[1])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm2  , mm[2])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm3  , mm[3])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm4  , mm[4])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm5  , mm[5])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm6  , mm[6])    //!< 64-bit MMX register.
ASMJIT_DEF_REG(X86MmReg , mm7  , mm[7])    //!< 64-bit MMX register.

ASMJIT_DEF_REG(X86SegReg, es   , seg[1])   //!< Cs segment register.
ASMJIT_DEF_REG(X86SegReg, cs   , seg[2])   //!< Ss segment register.
ASMJIT_DEF_REG(X86SegReg, ss   , seg[3])   //!< Ds segment register.
ASMJIT_DEF_REG(X86SegReg, ds   , seg[4])   //!< Es segment register.
ASMJIT_DEF_REG(X86SegReg, fs   , seg[5])   //!< Fs segment register.
ASMJIT_DEF_REG(X86SegReg, gs   , seg[6])   //!< Gs segment register.

ASMJIT_DEF_REG(X86GpReg , noGpReg, noGp)   //!< No GP register (for `X86Mem` operand).
ASMJIT_DEF_REG(X86RipReg, rip, rip)        //!< RIP register.

#undef ASMJIT_DEF_REG

//! Create 8-bit low GPB register operand.
static ASMJIT_INLINE X86GpReg gpb_lo(uint32_t index) noexcept { return X86GpReg(kX86RegTypeGpbLo, index, 1); }
//! Create 8-bit high GPB register operand.
static ASMJIT_INLINE X86GpReg gpb_hi(uint32_t index) noexcept { return X86GpReg(kX86RegTypeGpbHi, index, 1); }
//! Create 16-bit GPW register operand.
static ASMJIT_INLINE X86GpReg gpw(uint32_t index) noexcept { return X86GpReg(kX86RegTypeGpw, index, 2); }
//! Create 32-bit GPD register operand.
static ASMJIT_INLINE X86GpReg gpd(uint32_t index) noexcept { return X86GpReg(kX86RegTypeGpd, index, 4); }
//! Create 64-bit GPQ register operand (X64).
static ASMJIT_INLINE X86GpReg gpq(uint32_t index) noexcept { return X86GpReg(kX86RegTypeGpq, index, 8); }
//! Create 80-bit Fp register operand.
static ASMJIT_INLINE X86FpReg fp(uint32_t index) noexcept { return X86FpReg(kX86RegTypeFp, index, 10); }
//! Create 64-bit Mm register operand.
static ASMJIT_INLINE X86MmReg mm(uint32_t index) noexcept { return X86MmReg(kX86RegTypeMm, index, 8); }
//! Create 64-bit K register operand.
static ASMJIT_INLINE X86KReg k(uint32_t index) noexcept { return X86KReg(kX86RegTypeK, index, 8); }
//! Create 128-bit XMM register operand.
static ASMJIT_INLINE X86XmmReg xmm(uint32_t index) noexcept { return X86XmmReg(kX86RegTypeXmm, index, 16); }
//! Create 256-bit YMM register operand.
static ASMJIT_INLINE X86YmmReg ymm(uint32_t index) noexcept { return X86YmmReg(kX86RegTypeYmm, index, 32); }
//! Create 512-bit ZMM register operand.
static ASMJIT_INLINE X86ZmmReg zmm(uint32_t index) noexcept { return X86ZmmReg(kX86RegTypeZmm, index, 64); }

// ============================================================================
// [asmjit::x86 - Ptr (Reg)]
// ============================================================================

//! Create `[base.reg + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpReg& base, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, disp, size);
}
//! Create `[base.reg + (index.reg << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpReg& base, const X86GpReg& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, index, shift, disp, size);
}
//! Create `[base.reg + (xmm.reg << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpReg& base, const X86XmmReg& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, index, shift, disp, size);
}
//! Create `[base.reg + (ymm.reg << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpReg& base, const X86YmmReg& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, index, shift, disp, size);
}

//! Create `[label + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const Label& label, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(label, disp, size);
}
//! Create `[label + (index.reg << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const Label& label, const X86GpReg& index, uint32_t shift, int32_t disp = 0, uint32_t size = 0) noexcept { \
  return X86Mem(label, index, shift, disp, size); \
}

//! Create `[RIP + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86RipReg& rip_, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(rip_, disp, size);
}

//! Create `[p + disp]` absolute memory operand with no/custom size information.
ASMJIT_API X86Mem ptr_abs(Ptr p, int32_t disp = 0, uint32_t size = 0) noexcept;
//! Create `[p + (index.reg << shift) + disp]` absolute memory operand with no/custom size information.
ASMJIT_API X86Mem ptr_abs(Ptr p, const X86Reg& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept;

//! \internal
#define ASMJIT_EXPAND_PTR_REG(prefix, size) \
  /*! Create `[base.reg + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpReg& base, int32_t disp = 0) noexcept { \
    return X86Mem(base, disp, size); \
  } \
  /*! Create `[base.reg + (index.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpReg& base, const X86GpReg& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr(base, index, shift, disp, size); \
  } \
  /*! Create `[base.reg + (xmm.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpReg& base, const X86XmmReg& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr(base, index, shift, disp, size); \
  } \
  /*! Create `[base.reg + (ymm.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpReg& base, const X86YmmReg& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr(base, index, shift, disp, size); \
  } \
  /*! Create `[label + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const Label& label, int32_t disp = 0) noexcept { \
    return ptr(label, disp, size); \
  } \
  /*! Create `[label + (index.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const Label& label, const X86GpReg& index, uint32_t shift, int32_t disp = 0) noexcept { \
    return ptr(label, index, shift, disp, size); \
  } \
  /*! Create `[RIP + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86RipReg& rip_, int32_t disp = 0) noexcept { \
    return ptr(rip_, disp, size); \
  } \
  /*! Create `[p + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, int32_t disp = 0) noexcept { \
    return ptr_abs(p, disp, size); \
  } \
  /*! Create `[p + (index.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, const X86GpReg& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr_abs(p, index, shift, disp, size); \
  } \
  /*! Create `[p + (xmm.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, const X86XmmReg& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr_abs(p, index, shift, disp, size); \
  } \
  /*! Create `[p + (ymm.reg << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, const X86YmmReg& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr_abs(p, index, shift, disp, size); \
  }

ASMJIT_EXPAND_PTR_REG(byte, 1)
ASMJIT_EXPAND_PTR_REG(word, 2)
ASMJIT_EXPAND_PTR_REG(dword, 4)
ASMJIT_EXPAND_PTR_REG(qword, 8)
ASMJIT_EXPAND_PTR_REG(tword, 10)
ASMJIT_EXPAND_PTR_REG(oword, 16)
ASMJIT_EXPAND_PTR_REG(yword, 32)
ASMJIT_EXPAND_PTR_REG(zword, 64)
#undef ASMJIT_EXPAND_PTR_REG

// ============================================================================
// [asmjit::x86 - Ptr (Var)]
// ============================================================================

#if !defined(ASMJIT_DISABLE_COMPILER)
//! Create `[base.var + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpVar& base, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, disp, size);
}
//! Create `[base.var + (index.var << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpVar& base, const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, index, shift, disp, size);
}
//! Create `[base.var + (xmm.var << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpVar& base, const X86XmmVar& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, index, shift, disp, size);
}
//! Create `[base.var + (ymm.var << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const X86GpVar& base, const X86YmmVar& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept {
  return X86Mem(base, index, shift, disp, size);
}
//! Create `[label + (index.var << shift) + disp]` memory operand with no/custom size information.
static ASMJIT_INLINE X86Mem ptr(const Label& label, const X86GpVar& index, uint32_t shift, int32_t disp = 0, uint32_t size = 0) noexcept { \
  return X86Mem(label, index, shift, disp, size); \
}

//! Create `[p + (index.var << shift) + disp]` absolute memory operand with no/custom size information.
ASMJIT_API X86Mem ptr_abs(Ptr p, const X86Var& index, uint32_t shift = 0, int32_t disp = 0, uint32_t size = 0) noexcept;

//! \internal
#define ASMJIT_EXPAND_PTR_VAR(prefix, size) \
  /*! Create `[base.var + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpVar& base, int32_t disp = 0) noexcept { \
    return X86Mem(base, disp, size); \
  } \
  /*! Create `[base.var + (index.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpVar& base, const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr(base, index, shift, disp, size); \
  } \
  /*! Create `[base.var + (xmm.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpVar& base, const X86XmmVar& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr(base, index, shift, disp, size); \
  } \
  /*! Create `[base.var + (ymm.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const X86GpVar& base, const X86YmmVar& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr(base, index, shift, disp, size); \
  } \
  /*! Create `[label + (index.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr(const Label& label, const X86GpVar& index, uint32_t shift, int32_t disp = 0) noexcept { \
    return ptr(label, index, shift, disp, size); \
  } \
  /*! Create `[p + (index.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, const X86GpVar& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr_abs(p, reinterpret_cast<const X86Var&>(index), shift, disp, size); \
  } \
  /*! Create `[p + (xmm.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, const X86XmmVar& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr_abs(p, reinterpret_cast<const X86Var&>(index), shift, disp, size); \
  } \
  /*! Create `[p + (ymm.var << shift) + disp]` memory operand. */ \
  static ASMJIT_INLINE X86Mem prefix##_ptr##_abs(Ptr p, const X86YmmVar& index, uint32_t shift = 0, int32_t disp = 0) noexcept { \
    return ptr_abs(p, reinterpret_cast<const X86Var&>(index), shift, disp, size); \
  }

ASMJIT_EXPAND_PTR_VAR(byte, 1)
ASMJIT_EXPAND_PTR_VAR(word, 2)
ASMJIT_EXPAND_PTR_VAR(dword, 4)
ASMJIT_EXPAND_PTR_VAR(qword, 8)
ASMJIT_EXPAND_PTR_VAR(tword, 10)
ASMJIT_EXPAND_PTR_VAR(oword, 16)
ASMJIT_EXPAND_PTR_VAR(yword, 32)
ASMJIT_EXPAND_PTR_VAR(zword, 64)
#undef ASMJIT_EXPAND_PTR_VAR

#endif // !ASMJIT_DISABLE_COMPILER

} // x86 namespace

#endif // !ASMJIT_EXPORTS_X86_REGS

//! \}

} // asmjit namespace

// [Cleanup]
#undef ASMJIT_OP_ID

// [Api-End]
#include "../apiend.h"

// [Guard]
#endif // _ASMJIT_X86_X86OPERAND_H
