#include "CodeGenerator.hpp"

#include "Error.hpp"

namespace SoftWire
{
	CodeGenerator::CodeGenerator()
	{
		physicalEAX = 0;
		physicalECX = 0;
		physicalEDX = 0;
		physicalEBX = 0;
		physicalESI = 0;
		physicalEDI = 0;

		physicalMM0 = 0;
		physicalMM1 = 0;
		physicalMM2 = 0;
		physicalMM3 = 0;
		physicalMM4 = 0;
		physicalMM5 = 0;
		physicalMM6 = 0;
		physicalMM7 = 0;

		physicalXMM0 = 0;
		physicalXMM1 = 0;
		physicalXMM2 = 0;
		physicalXMM3 = 0;
		physicalXMM4 = 0;
		physicalXMM5 = 0;
		physicalXMM6 = 0;
		physicalXMM7 = 0;

		priorityEAX = 0;
		priorityECX = 0;
		priorityEDX = 0;
		priorityEBX = 0;
		priorityESI = 0;
		priorityEDI = 0;

		priorityMM0 = 0;
		priorityMM1 = 0;
		priorityMM2 = 0;
		priorityMM3 = 0;
		priorityMM4 = 0;
		priorityMM5 = 0;
		priorityMM6 = 0;
		priorityMM7 = 0;

		priorityXMM0 = 0;
		priorityXMM1 = 0;
		priorityXMM2 = 0;
		priorityXMM3 = 0;
		priorityXMM4 = 0;
		priorityXMM5 = 0;
		priorityXMM6 = 0;
		priorityXMM7 = 0;
	}

	const OperandREG32 &CodeGenerator::r32(const OperandREF &ref, bool copy)
	{
		if(ref == 0 && copy) throw Error("Cannot dereference 0");

		// Check if already allocated
		     if(physicalEAX == ref) return access(eax);
		else if(physicalECX == ref) return access(ecx);
		else if(physicalEDX == ref) return access(edx);
		else if(physicalEBX == ref) return access(ebx);
		else if(physicalESI == ref) return access(esi);
		else if(physicalEDI == ref) return access(edi);

		// Search for free registers
		     if(physicalEAX == 0 && priorityEAX == 0) return assign(eax, ref, copy);
		else if(physicalECX == 0 && priorityECX == 0) return assign(ecx, ref, copy);
		else if(physicalEDX == 0 && priorityEDX == 0) return assign(edx, ref, copy);
		else if(physicalEBX == 0 && priorityEBX == 0) return assign(ebx, ref, copy);
		else if(physicalESI == 0 && priorityESI == 0) return assign(esi, ref, copy);
		else if(physicalEDI == 0 && priorityEDI == 0) return assign(edi, ref, copy);

		// Need to spill one
		Encoding::Reg candidate = Encoding::REG_UNKNOWN;
		unsigned int priority = 0xFFFFFFFF;

		if(priorityEAX < priority && real(physicalEAX)) {priority = priorityEAX; candidate = Encoding::EAX;}
		if(priorityECX < priority && real(physicalECX)) {priority = priorityECX; candidate = Encoding::ECX;}
		if(priorityEDX < priority && real(physicalEDX)) {priority = priorityEDX; candidate = Encoding::EDX;}
		if(priorityEBX < priority && real(physicalEBX)) {priority = priorityEBX; candidate = Encoding::EBX;}
		if(priorityESI < priority && real(physicalESI)) {priority = priorityESI; candidate = Encoding::ESI;}
		if(priorityEDI < priority && real(physicalEDI)) {priority = priorityEDI; candidate = Encoding::EDI;}

		switch(candidate)
		{
		case Encoding::REG_UNKNOWN: throw Error("Out of physical general purpose registers. Use free().");
		case Encoding::EAX: spill(eax); return assign(eax, ref, copy);
		case Encoding::ECX: spill(ecx); return assign(ecx, ref, copy);
		case Encoding::EDX: spill(edx); return assign(edx, ref, copy);
		case Encoding::EBX: spill(ebx); return assign(ebx, ref, copy);
		case Encoding::ESI: spill(esi); return assign(esi, ref, copy);
		case Encoding::EDI: spill(edi); return assign(edi, ref, copy);
		default: throw INTERNAL_ERROR;
		}
	}

	const OperandREG32 &CodeGenerator::x32(const OperandREF &ref, bool copy)
	{
		return r32(ref, copy);
	}

	const OperandREG32 &CodeGenerator::t32(int i)
	{
		if(i < 0 || i >= 6) throw Error("Register allocator t32 index out of range");

		return x32(i);
	}

	const OperandR_M32 CodeGenerator::m32(const OperandREF &ref)
	{
		if(ref == 0) throw Error("Cannot dereference 0");

		// Check if already allocated
		     if(physicalEAX == ref) return access(eax);
		else if(physicalECX == ref) return access(ecx);
		else if(physicalEDX == ref) return access(edx);
		else if(physicalEBX == ref) return access(ebx);
		else if(physicalESI == ref) return access(esi);
		else if(physicalEDI == ref) return access(edi);

		return dword_ptr [ref];
	}

	const OperandREG32 &CodeGenerator::allocate(const OperandREG32 &reg, const OperandREF &ref, bool copy)
	{
		return assign(reg, ref, copy);
	}

	const OperandREG32 &CodeGenerator::assign(const OperandREG32 &reg, const OperandREF &ref, bool copy)
	{
		switch(reg.reg)
		{
		case Encoding::EAX:
			if(physicalEAX != 0) throw Error("eax not available for register allocation");
			physicalEAX = ref;
			priorityEAX = 0xFFFFFFFF;
			if(copy && real(ref)) mov(eax, dword_ptr [ref]);
			return access(eax);
		case Encoding::ECX:
			if(physicalECX != 0) throw Error("ecx not available for register allocation");
			physicalECX = ref;
			priorityECX = 0xFFFFFFFF;
			if(copy && real(ref)) mov(ecx, dword_ptr [ref]);
			return access(ecx);
		case Encoding::EDX:
			if(physicalEDX != 0) throw Error("edx not available for register allocation");
			physicalEDX = ref;
			priorityEDX = 0xFFFFFFFF;
			if(copy && real(ref)) mov(edx, dword_ptr [ref]);
			return access(edx);
		case Encoding::EBX:
			if(physicalEBX != 0) throw Error("ebx not available for register allocation");
			physicalEBX = ref;
			priorityEBX = 0xFFFFFFFF;
			if(copy && real(ref)) mov(ebx, dword_ptr [ref]);
			return access(ebx);
		case Encoding::ESI:
			if(physicalESI != 0) throw Error("esi not available for register allocation");
			physicalESI = ref;
			priorityESI = 0xFFFFFFFF;
			if(copy && real(ref)) mov(esi, dword_ptr [ref]);
			return access(esi);
		case Encoding::EDI:
			if(physicalEDI != 0) throw Error("edi not available for register allocation");
			physicalEDI = ref;
			priorityEDI = 0xFFFFFFFF;
			if(copy && real(ref)) mov(edi, dword_ptr [ref]);
			return access(edi);
		default: throw INTERNAL_ERROR;
		}
	}

	const OperandREG32 &CodeGenerator::access(const OperandREG32 &reg)
	{
		// Decrease priority of other registers
		if(reg.reg != Encoding::EAX && priorityEAX) priorityEAX--;
		if(reg.reg != Encoding::ECX && priorityECX) priorityECX--;
		if(reg.reg != Encoding::EDX && priorityEDX) priorityEDX--;
		if(reg.reg != Encoding::EBX && priorityEBX) priorityEBX--;
		if(reg.reg != Encoding::ESI && priorityESI) priorityESI--;
		if(reg.reg != Encoding::EDI && priorityEDI) priorityEDI--;

		return reg;
	}

	void CodeGenerator::free(const OperandREG32 &reg)
	{
		switch(reg.reg)
		{
		case Encoding::EAX: physicalEAX = 0; priorityEAX = 0; break;
		case Encoding::ECX: physicalECX = 0; priorityECX = 0; break;
		case Encoding::EDX: physicalEDX = 0; priorityEDX = 0; break;
		case Encoding::EBX: physicalEBX = 0; priorityEBX = 0; break;
		case Encoding::ESI: physicalESI = 0; priorityESI = 0; break;
		case Encoding::EDI: physicalEDI = 0; priorityEDI = 0; break;
		default: throw INTERNAL_ERROR;
		}
	}

	void CodeGenerator::spill(const OperandREG32 &reg)
	{
		switch(reg.reg)
		{
		case Encoding::EAX: if(real(physicalEAX)) mov(dword_ptr [physicalEAX], eax); break;
		case Encoding::ECX: if(real(physicalECX)) mov(dword_ptr [physicalECX], ecx); break;
		case Encoding::EDX: if(real(physicalEDX)) mov(dword_ptr [physicalEDX], edx); break;
		case Encoding::EBX: if(real(physicalEBX)) mov(dword_ptr [physicalEBX], ebx); break;
		case Encoding::ESI: if(real(physicalESI)) mov(dword_ptr [physicalESI], esi); break;
		case Encoding::EDI: if(real(physicalEDI)) mov(dword_ptr [physicalEDI], edi); break;
		default: throw INTERNAL_ERROR;
		}

		free(reg);
	}

	const OperandMMREG &CodeGenerator::r64(const OperandREF &ref, bool copy)
	{
		if(ref == 0 && copy) throw Error("Cannot dereference 0");

		// Check if already allocated
		     if(physicalMM0 == ref) return access(mm0);
		else if(physicalMM1 == ref) return access(mm1);
		else if(physicalMM2 == ref) return access(mm2);
		else if(physicalMM3 == ref) return access(mm3);
		else if(physicalMM4 == ref) return access(mm4);
		else if(physicalMM5 == ref) return access(mm5);
		else if(physicalMM6 == ref) return access(mm6);
		else if(physicalMM7 == ref) return access(mm7);

		// Search for free registers
		     if(physicalMM0 == 0 && priorityMM0 == 0) return assign(mm0, ref, copy);
		else if(physicalMM1 == 0 && priorityMM1 == 0) return assign(mm1, ref, copy);
		else if(physicalMM2 == 0 && priorityMM2 == 0) return assign(mm2, ref, copy);
		else if(physicalMM3 == 0 && priorityMM3 == 0) return assign(mm3, ref, copy);
		else if(physicalMM4 == 0 && priorityMM4 == 0) return assign(mm4, ref, copy);
		else if(physicalMM5 == 0 && priorityMM5 == 0) return assign(mm5, ref, copy);
		else if(physicalMM6 == 0 && priorityMM6 == 0) return assign(mm6, ref, copy);
		else if(physicalMM7 == 0 && priorityMM7 == 0) return assign(mm7, ref, copy);

		// Need to spill one
		Encoding::Reg candidate = Encoding::REG_UNKNOWN;
		unsigned int priority = 0xFFFFFFFF;

		if(priorityMM0 < priority && real(physicalMM0)) {priority = priorityMM0; candidate = Encoding::MM0;}
		if(priorityMM1 < priority && real(physicalMM1)) {priority = priorityMM1; candidate = Encoding::MM1;}
		if(priorityMM2 < priority && real(physicalMM2)) {priority = priorityMM2; candidate = Encoding::MM2;}
		if(priorityMM3 < priority && real(physicalMM3)) {priority = priorityMM3; candidate = Encoding::MM3;}
		if(priorityMM4 < priority && real(physicalMM4)) {priority = priorityMM4; candidate = Encoding::MM4;}
		if(priorityMM5 < priority && real(physicalMM5)) {priority = priorityMM5; candidate = Encoding::MM5;}
		if(priorityMM6 < priority && real(physicalMM6)) {priority = priorityMM6; candidate = Encoding::MM6;}
		if(priorityMM7 < priority && real(physicalMM7)) {priority = priorityMM7; candidate = Encoding::MM7;}

		switch(candidate)
		{
		case Encoding::REG_UNKNOWN: throw Error("Out of physical MMX registers. Use free().");
		case Encoding::MM0: spill(mm0); return assign(mm0, ref, copy);
		case Encoding::MM1: spill(mm1); return assign(mm1, ref, copy);
		case Encoding::MM2: spill(mm2); return assign(mm2, ref, copy);
		case Encoding::MM3: spill(mm3); return assign(mm3, ref, copy);
		case Encoding::MM4: spill(mm4); return assign(mm4, ref, copy);
		case Encoding::MM5: spill(mm5); return assign(mm5, ref, copy);
		case Encoding::MM6: spill(mm6); return assign(mm6, ref, copy);
		case Encoding::MM7: spill(mm7); return assign(mm7, ref, copy);
		default: throw INTERNAL_ERROR;
		}
	}

	const OperandMMREG &CodeGenerator::x64(const OperandREF &ref, bool copy)
	{
		return r64(ref, copy);
	}

	const OperandMMREG &CodeGenerator::t64(int i)
	{
		if(i < 0 || i >= 8) throw Error("Register allocator t64 index out of range");

		return x64(i);
	}

	const OperandR_M64 CodeGenerator::m64(const OperandREF &ref)
	{
		if(ref == 0) throw Error("Cannot dereference 0");

		// Check if already allocated
		     if(physicalMM0 == ref) return access(mm0);
		else if(physicalMM1 == ref) return access(mm1);
		else if(physicalMM2 == ref) return access(mm2);
		else if(physicalMM3 == ref) return access(mm3);
		else if(physicalMM4 == ref) return access(mm4);
		else if(physicalMM5 == ref) return access(mm5);
		else if(physicalMM6 == ref) return access(mm6);
		else if(physicalMM7 == ref) return access(mm7);

		return qword_ptr [ref];
	}

	const OperandMMREG &CodeGenerator::allocate(const OperandMMREG &reg, const OperandREF &ref, bool copy)
	{
		return assign(reg, ref, copy);
	}

	const OperandMMREG &CodeGenerator::assign(const OperandMMREG &reg, const OperandREF &ref, bool copy)
	{
		switch(reg.reg)
		{
		case Encoding::MM0:
			if(physicalMM0 != 0) throw Error("mm0 not available for register allocation");
			physicalMM0 = ref;
			priorityMM0 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm0, qword_ptr [ref]);
			return access(mm0);
		case Encoding::MM1:
			if(physicalMM1 != 0) throw Error("mm1 not available for register allocation");
			physicalMM1 = ref;
			priorityMM1 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm1, qword_ptr [ref]);
			return access(mm1);
		case Encoding::MM2:
			if(physicalMM2 != 0) throw Error("mm2 not available for register allocation");
			physicalMM2 = ref;
			priorityMM2 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm2, qword_ptr [ref]);
			return access(mm2);
		case Encoding::MM3:
			if(physicalMM3 != 0) throw Error("mm3 not available for register allocation");
			physicalMM3 = ref;
			priorityMM3 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm3, qword_ptr [ref]);
			return access(mm3);
		case Encoding::MM4:
			if(physicalMM4 != 0) throw Error("mm4 not available for register allocation");
			physicalMM4 = ref;
			priorityMM4 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm4, qword_ptr [ref]);
			return access(mm4);
		case Encoding::MM5:
			if(physicalMM5 != 0) throw Error("mm5 not available for register allocation");
			physicalMM5 = ref;
			priorityMM5 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm5, qword_ptr [ref]);
			return access(mm5);
		case Encoding::MM6:
			if(physicalMM6 != 0) throw Error("mm6 not available for register allocation");
			physicalMM6 = ref;
			priorityMM6 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm6, qword_ptr [ref]);
			return access(mm6);
		case Encoding::MM7:
			if(physicalMM7 != 0) throw Error("mm7 not available for register allocation");
			physicalMM7 = ref;
			priorityMM7 = 0xFFFFFFFF;
			if(copy && real(ref)) movq(mm7, qword_ptr [ref]);
			return access(mm7);
		default: throw INTERNAL_ERROR;
		}
	}

	const OperandMMREG &CodeGenerator::access(const OperandMMREG &reg)
	{
		// Decrease priority of other registers
		if(reg.reg != Encoding::MM0 && priorityMM0) priorityMM0--;
		if(reg.reg != Encoding::MM1 && priorityMM1) priorityMM1--;
		if(reg.reg != Encoding::MM2 && priorityMM2) priorityMM2--;
		if(reg.reg != Encoding::MM3 && priorityMM3) priorityMM3--;
		if(reg.reg != Encoding::MM4 && priorityMM4) priorityMM4--;
		if(reg.reg != Encoding::MM5 && priorityMM5) priorityMM5--;
		if(reg.reg != Encoding::MM6 && priorityMM6) priorityMM6--;
		if(reg.reg != Encoding::MM7 && priorityMM7) priorityMM7--;

		return reg;
	}

	void CodeGenerator::free(const OperandMMREG &reg)
	{
		switch(reg.reg)
		{
		case Encoding::MM0: physicalMM0 = 0; priorityMM0 = 0; break;
		case Encoding::MM1: physicalMM1 = 0; priorityMM1 = 0; break;
		case Encoding::MM2: physicalMM2 = 0; priorityMM2 = 0; break;
		case Encoding::MM3: physicalMM3 = 0; priorityMM3 = 0; break;
		case Encoding::MM4: physicalMM4 = 0; priorityMM4 = 0; break;
		case Encoding::MM5: physicalMM5 = 0; priorityMM5 = 0; break;
		case Encoding::MM6: physicalMM6 = 0; priorityMM6 = 0; break;
		case Encoding::MM7: physicalMM7 = 0; priorityMM7 = 0; break;
		default: throw INTERNAL_ERROR;
		}
	}

	void CodeGenerator::spill(const OperandMMREG &reg)
	{
		switch(reg.reg)
		{
		case Encoding::MM0: if(real(physicalMM0)) movq(qword_ptr [physicalMM0], mm0); break;
		case Encoding::MM1: if(real(physicalMM1)) movq(qword_ptr [physicalMM1], mm1); break;
		case Encoding::MM2: if(real(physicalMM2)) movq(qword_ptr [physicalMM2], mm2); break;
		case Encoding::MM3: if(real(physicalMM3)) movq(qword_ptr [physicalMM3], mm3); break;
		case Encoding::MM4: if(real(physicalMM4)) movq(qword_ptr [physicalMM4], mm4); break;
		case Encoding::MM5: if(real(physicalMM5)) movq(qword_ptr [physicalMM5], mm5); break;
		case Encoding::MM6: if(real(physicalMM6)) movq(qword_ptr [physicalMM6], mm6); break;
		case Encoding::MM7: if(real(physicalMM7)) movq(qword_ptr [physicalMM7], mm7); break;
		default: throw INTERNAL_ERROR;
		}

		free(reg);
	}

	const OperandXMMREG &CodeGenerator::r128(const OperandREF &ref, bool copy)
	{
		if(ref == 0 && copy) throw Error("Cannot dereference 0");

		// Check if already allocated
		     if(physicalXMM0 == ref) return access(xmm0);
		else if(physicalXMM1 == ref) return access(xmm1);
		else if(physicalXMM2 == ref) return access(xmm2);
		else if(physicalXMM3 == ref) return access(xmm3);
		else if(physicalXMM4 == ref) return access(xmm4);
		else if(physicalXMM5 == ref) return access(xmm5);
		else if(physicalXMM6 == ref) return access(xmm6);
		else if(physicalXMM7 == ref) return access(xmm7);

		// Search for free registers
		     if(physicalXMM0 == 0 && priorityXMM0 == 0) return assign(xmm0, ref, copy);
		else if(physicalXMM1 == 0 && priorityXMM1 == 0) return assign(xmm1, ref, copy);
		else if(physicalXMM2 == 0 && priorityXMM2 == 0) return assign(xmm2, ref, copy);
		else if(physicalXMM3 == 0 && priorityXMM3 == 0) return assign(xmm3, ref, copy);
		else if(physicalXMM4 == 0 && priorityXMM4 == 0) return assign(xmm4, ref, copy);
		else if(physicalXMM5 == 0 && priorityXMM5 == 0) return assign(xmm5, ref, copy);
		else if(physicalXMM6 == 0 && priorityXMM6 == 0) return assign(xmm6, ref, copy);
		else if(physicalXMM7 == 0 && priorityXMM7 == 0) return assign(xmm7, ref, copy);

		// Need to spill one
		Encoding::Reg candidate = Encoding::REG_UNKNOWN;
		unsigned int priority = 0xFFFFFFFF;

		if(priorityXMM0 < priority && real(physicalXMM0)) {priority = priorityXMM0; candidate = Encoding::XMM0;}
		if(priorityXMM1 < priority && real(physicalXMM1)) {priority = priorityXMM1; candidate = Encoding::XMM1;}
		if(priorityXMM2 < priority && real(physicalXMM2)) {priority = priorityXMM2; candidate = Encoding::XMM2;}
		if(priorityXMM3 < priority && real(physicalXMM3)) {priority = priorityXMM3; candidate = Encoding::XMM3;}
		if(priorityXMM4 < priority && real(physicalXMM4)) {priority = priorityXMM4; candidate = Encoding::XMM4;}
		if(priorityXMM5 < priority && real(physicalXMM5)) {priority = priorityXMM5; candidate = Encoding::XMM5;}
		if(priorityXMM6 < priority && real(physicalXMM6)) {priority = priorityXMM6; candidate = Encoding::XMM6;}
		if(priorityXMM7 < priority && real(physicalXMM7)) {priority = priorityXMM7; candidate = Encoding::XMM7;}

		switch(candidate)
		{
		case Encoding::REG_UNKNOWN: throw Error("Out of physical SSE registers. Use free().");
		case Encoding::XMM0: spill(xmm0); return assign(xmm0, ref, copy);
		case Encoding::XMM1: spill(xmm1); return assign(xmm1, ref, copy);
		case Encoding::XMM2: spill(xmm2); return assign(xmm2, ref, copy);
		case Encoding::XMM3: spill(xmm3); return assign(xmm3, ref, copy);
		case Encoding::XMM4: spill(xmm4); return assign(xmm4, ref, copy);
		case Encoding::XMM5: spill(xmm5); return assign(xmm5, ref, copy);
		case Encoding::XMM6: spill(xmm6); return assign(xmm6, ref, copy);
		case Encoding::XMM7: spill(xmm7); return assign(xmm7, ref, copy);
		default: throw INTERNAL_ERROR;
		}
	}

	const OperandXMMREG &CodeGenerator::x128(const OperandREF &ref, bool copy)
	{
		return r128(ref, copy);
	}

	const OperandXMMREG &CodeGenerator::t128(int i)
	{
		if(i < 0 || i >= 8) throw Error("Register allocator t128 index out of range");

		return x128(i);
	}

	const OperandR_M128 CodeGenerator::m128(const OperandREF &ref)
	{
		if(ref == 0) throw Error("Cannot dereference 0");

		// Check if already allocated
		     if(physicalXMM0 == ref) return access(xmm0);
		else if(physicalXMM1 == ref) return access(xmm1);
		else if(physicalXMM2 == ref) return access(xmm2);
		else if(physicalXMM3 == ref) return access(xmm3);
		else if(physicalXMM4 == ref) return access(xmm4);
		else if(physicalXMM5 == ref) return access(xmm5);
		else if(physicalXMM6 == ref) return access(xmm6);
		else if(physicalXMM7 == ref) return access(xmm7);

		return xword_ptr [ref];
	}

	const OperandXMMREG &CodeGenerator::allocate(const OperandXMMREG &reg, const OperandREF &ref, bool copy)
	{
		return assign(reg, ref, copy);
	}

	const OperandXMMREG &CodeGenerator::assign(const OperandXMMREG &reg, const OperandREF &ref, bool copy)
	{
		switch(reg.reg)
		{
		case Encoding::XMM0:
			if(physicalXMM0 != 0) throw Error("xmm0 not available for register allocation");
			physicalXMM0 = ref;
			priorityXMM0 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm0, xword_ptr [ref]);
			return access(xmm0);
		case Encoding::XMM1:
			if(physicalXMM1 != 0) throw Error("xmm1 not available for register allocation");
			physicalXMM1 = ref;
			priorityXMM1 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm1, xword_ptr [ref]);
			return access(xmm1);
		case Encoding::XMM2:
			if(physicalXMM2 != 0) throw Error("xmm2 not available for register allocation");
			physicalXMM2 = ref;
			priorityXMM2 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm2, xword_ptr [ref]);
			return access(xmm2);
		case Encoding::XMM3:
			if(physicalXMM3 != 0) throw Error("xmm3 not available for register allocation");
			physicalXMM3 = ref;
			priorityXMM3 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm3, xword_ptr [ref]);
			return access(xmm3);
		case Encoding::XMM4:
			if(physicalXMM4 != 0) throw Error("xmm4 not available for register allocation");
			physicalXMM4 = ref;
			priorityXMM4 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm4, xword_ptr [ref]);
			return access(xmm4);
		case Encoding::XMM5:
			if(physicalXMM5 != 0) throw Error("xmm5 not available for register allocation");
			physicalXMM5 = ref;
			priorityXMM5 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm5, xword_ptr [ref]);
			return access(xmm5);
		case Encoding::XMM6:
			if(physicalXMM6 != 0) throw Error("xmm6 not available for register allocation");
			physicalXMM6 = ref;
			priorityXMM6 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm6, xword_ptr [ref]);
			return access(xmm6);
		case Encoding::XMM7:
			if(physicalXMM7 != 0) throw Error("xmm7 not available for register allocation");
			physicalXMM7 = ref;
			priorityXMM7 = 0xFFFFFFFF;
			if(copy && real(ref)) movaps(xmm7, xword_ptr [ref]);
			return access(xmm7);
		default: throw INTERNAL_ERROR;
		}
	}

	const OperandXMMREG &CodeGenerator::access(const OperandXMMREG &reg)
	{
		// Decrease priority of other registers
		if(reg.reg != Encoding::XMM0 && priorityXMM0) priorityXMM0--;
		if(reg.reg != Encoding::XMM1 && priorityXMM1) priorityXMM1--;
		if(reg.reg != Encoding::XMM2 && priorityXMM2) priorityXMM2--;
		if(reg.reg != Encoding::XMM3 && priorityXMM3) priorityXMM3--;
		if(reg.reg != Encoding::XMM4 && priorityXMM4) priorityXMM4--;
		if(reg.reg != Encoding::XMM5 && priorityXMM5) priorityXMM5--;
		if(reg.reg != Encoding::XMM6 && priorityXMM6) priorityXMM6--;
		if(reg.reg != Encoding::XMM7 && priorityXMM7) priorityXMM7--;

		return reg;
	}

	void CodeGenerator::free(const OperandXMMREG &reg)
	{
		switch(reg.reg)
		{
		case Encoding::XMM0: physicalXMM0 = 0; priorityXMM0 = 0; break;
		case Encoding::XMM1: physicalXMM1 = 0; priorityXMM1 = 0; break;
		case Encoding::XMM2: physicalXMM2 = 0; priorityXMM2 = 0; break;
		case Encoding::XMM3: physicalXMM3 = 0; priorityXMM3 = 0; break;
		case Encoding::XMM4: physicalXMM4 = 0; priorityXMM4 = 0; break;
		case Encoding::XMM5: physicalXMM5 = 0; priorityXMM5 = 0; break;
		case Encoding::XMM6: physicalXMM6 = 0; priorityXMM6 = 0; break;
		case Encoding::XMM7: physicalXMM7 = 0; priorityXMM7 = 0; break;
		default: throw INTERNAL_ERROR;
		}
	}

	void CodeGenerator::spill(const OperandXMMREG &reg)
	{
		switch(reg.reg)
		{
		case Encoding::XMM0: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM0], xmm0); break;
		case Encoding::XMM1: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM1], xmm1); break;
		case Encoding::XMM2: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM2], xmm2); break;
		case Encoding::XMM3: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM3], xmm3); break;
		case Encoding::XMM4: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM4], xmm4); break;
		case Encoding::XMM5: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM5], xmm5); break;
		case Encoding::XMM6: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM6], xmm6); break;
		case Encoding::XMM7: if(real(physicalXMM0)) movaps(xword_ptr [physicalXMM7], xmm7); break;
		default: throw INTERNAL_ERROR;
		}

		free(reg);
	}

	bool CodeGenerator::real(const OperandREF &ref)
	{
		return ref.baseReg != Encoding::REG_UNKNOWN ||
		       ref.indexReg != Encoding::REG_UNKNOWN ||
		       ref.scale != 0 ||
		       ref.displacement >= 8;
	}

	void CodeGenerator::free(const OperandREF &ref)
	{
		     if(physicalEAX == ref) free(eax);
		else if(physicalECX == ref) free(ecx);
		else if(physicalEDX == ref) free(edx);
		else if(physicalEBX == ref) free(ebx);
		else if(physicalESI == ref) free(esi);
		else if(physicalEDI == ref) free(edi);

		else if(physicalMM0 == ref) free(mm0);
		else if(physicalMM1 == ref) free(mm1);
		else if(physicalMM2 == ref) free(mm2);
		else if(physicalMM3 == ref) free(mm3);
		else if(physicalMM4 == ref) free(mm4);
		else if(physicalMM5 == ref) free(mm5);
		else if(physicalMM6 == ref) free(mm6);
		else if(physicalMM7 == ref) free(mm7);
	}

	void CodeGenerator::spill(const OperandREF &ref)
	{
		     if(physicalEAX == ref) mov(dword_ptr [physicalEAX], eax);
		else if(physicalECX == ref) mov(dword_ptr [physicalECX], ecx);
		else if(physicalEDX == ref) mov(dword_ptr [physicalEDX], edx);
		else if(physicalEBX == ref) mov(dword_ptr [physicalEBX], ebx);
		else if(physicalESI == ref) mov(dword_ptr [physicalESI], esi);
		else if(physicalEDI == ref) mov(dword_ptr [physicalEDI], edi);

		else if(physicalMM0 == ref) movq(qword_ptr [physicalMM0], mm0);
		else if(physicalMM1 == ref) movq(qword_ptr [physicalMM1], mm1);
		else if(physicalMM2 == ref) movq(qword_ptr [physicalMM2], mm2);
		else if(physicalMM3 == ref) movq(qword_ptr [physicalMM3], mm3);
		else if(physicalMM4 == ref) movq(qword_ptr [physicalMM4], mm4);
		else if(physicalMM5 == ref) movq(qword_ptr [physicalMM5], mm5);
		else if(physicalMM6 == ref) movq(qword_ptr [physicalMM6], mm6);
		else if(physicalMM7 == ref) movq(qword_ptr [physicalMM7], mm7);

		free(ref);
	}

	void CodeGenerator::freeAll()
	{
		free(eax);
		free(ecx);
		free(edx);
		free(ebx);
		free(esi);
		free(edi);

		free(mm0);
		free(mm1);
		free(mm2);
		free(mm3);
		free(mm4);
		free(mm5);
		free(mm6);
		free(mm7);

		free(xmm0);
		free(xmm1);
		free(xmm2);
		free(xmm3);
		free(xmm4);
		free(xmm5);
		free(xmm6);
		free(xmm7);
	}

	void CodeGenerator::spillAll()
	{
		spill(eax);
		spill(ecx);
		spill(edx);
		spill(ebx);
		spill(esi);
		spill(edi);

		spill(mm0);
		spill(mm1);
		spill(mm2);
		spill(mm3);
		spill(mm4);
		spill(mm5);
		spill(mm6);
		spill(mm7);

		spill(xmm0);
		spill(xmm1);
		spill(xmm2);
		spill(xmm3);
		spill(xmm4);
		spill(xmm5);
		spill(xmm6);
		spill(xmm7);
	}
}
