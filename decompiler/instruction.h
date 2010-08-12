/* ScummVM Tools
 * Copyright (C) 2010 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#ifndef DEC_INSTRUCTION_H
#define DEC_INSTRUCTION_H

#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/variant.hpp>

#include "common/scummsys.h"

/**
 * Enumeration for categorizing the different kinds of instructions.
 */
enum InstType {
	kBinaryOp,    ///< Binary operation (e.g. +, &&, etc.), including comparisons.
	kCall,        ///< Regular function call.
	kCondJump,    ///< Conditional jump (absolute address).
	kCondJumpRel, ///< Conditional jump (relative address).
	kDup,         ///< Instruction duplicates the most recent stack entry.
	kJump,        ///< Unconditional jump (absolute address).
	kJumpRel,     ///< Unconditional jump (relative address).
	kLoad,        ///< Load value to stack.
	kReturn,      ///< Return from regular function call.
	kSpecial,     ///< Special functions.
	kStack,       ///< Stack allocation or deallocation (altering stack pointer).
	kStore,       ///< Store value from stack in memory.
	kUnaryOp      ///< Unary operation (e.g. !)
};

/**
 * Enumeration for categorizing the different kinds of parameters.
 */
enum ParamType {
	kSByte,  ///< Signed 8-bit integer.
	kByte,   ///< Unsigned 8-bit integer.
	kShort,  ///< Signed 16-bit integer.
	kUShort, ///< Unsigned 16-bit integer.
	kInt,    ///< Signed 32-bit integer.
	kUInt,   ///< Unsigned 32-bit integer.
	kString  ///< Text string.
};

/**
 * Structure for representing a parameter.
 */
struct Parameter {
	ParamType _type;                                   ///< Type of the parameter.
	boost::variant<int32, uint32, std::string> _value; ///< Value of the parameter.

	/**
	 * Gets an int32 stored in the _value variant.
	 *
	 * @return The int32 stored in the _value variant.
	 * @throws boost::bad_get if the variant is not storing an int32.
	 */
	int32 getSigned() const { return boost::get<int32>(_value); }

	/**
	 * Gets an uint32 stored in the _value variant.
	 *
	 * @return The uint32 stored in the _value variant.
	 * @throws boost::bad_get if the variant is not storing an uint32.
	 */
	uint32 getUnsigned() const { return boost::get<uint32>(_value); }

	/**
	 * Gets an std::string stored in the _value variant.
	 *
	 * @return The std::string stored in the _value variant.
	 * @throws boost::bad_get if the variant is not storing an std::string.
	 */
	std::string getString() const { return boost::get<std::string>(_value); }
};

/**
 * Structure for representing an instruction.
 */
struct Instruction {
	uint32 _opcode;                 ///< The instruction opcode.
	uint32 _address;                ///< The instruction address.
	int16 _stackChange;             ///< How much this instruction changes the stack pointer by.
	std::string _name;              ///< The instruction name (opcode name).
	InstType _type;                 ///< The instruction type.
	std::vector<Parameter> _params; ///< Array of parameters used for the instruction.
	std::string _codeGenData;       ///< String containing metadata for code generation. Start with 0xC0 to force custom handling. See the extended documentation for details.

	/**
	 * Operator overload to output a vector to a std::ostream.
	 *
	 * @param output The std::ostream to output to.
	 * @param inst   The Instruction to output.
	 * @return The std::ostream used for output.
	 */
	friend std::ostream &operator<<(std::ostream &output, const Instruction &inst) {
		output << boost::format("%08x: %s") % inst._address % inst._name;
		std::vector<Parameter>::const_iterator param;
		for (param = inst._params.begin(); param != inst._params.end(); ++param) {
			if (param != inst._params.begin())
				output << ",";
			if (inst._type == kCondJump || inst._type == kCondJumpRel || inst._type == kJump || inst._type == kJumpRel || inst._type == kCall) {
				switch (param->_type) {
				case kSByte:
				case kShort:
				case kInt:
					output << boost::format(" 0x%X") % param->getSigned();
					break;
				case kByte:
				case kUShort:
				case kUInt:
					output << boost::format(" 0x%X") % param->getUnsigned();
					break;
				default:
					output << " " << param->_value;
					break;
				}
			} else
				output << " " << param->_value;
		}
		output << boost::format(" (%d)") % inst._stackChange << "\n";
		return output;
	}
};

/**
 * Type representing an iterator over Instructions.
 */
typedef std::vector<Instruction>::iterator InstIterator;

/**
 * Type representing a const_iterator over Instructions.
 */
typedef std::vector<Instruction>::const_iterator ConstInstIterator;

#endif
