/*  This file is part of the GNU simulators.

    Copyright (C) 1994-1995,1997, Andrew Cagney <cagney@highland.com.au>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#include "sim-state.h"


int current_host_byte_order;
int current_target_byte_order;
int current_stdio;

#if defined (WITH_ENVIRONMENT)
int current_environment;
#endif

#if defined (WITH_ALIGNMENT)
int current_alignment;
#endif

#if defined (WITH_FLOATING_POINT)
int current_floating_point;
#endif



/* map a byte order onto a textual string */

static const char *
config_byte_order_to_a (int byte_order)
{
  switch (byte_order)
    {
    case LITTLE_ENDIAN:
      return "LITTLE_ENDIAN";
    case BIG_ENDIAN:
      return "BIG_ENDIAN";
    case 0:
      return "0";
    }
  return "UNKNOWN";
}


static const char *
config_stdio_to_a (int stdio)
{
  switch (stdio)
    {
    case DONT_USE_STDIO:
      return "DONT_USE_STDIO";
    case DO_USE_STDIO:
      return "DO_USE_STDIO";
    case 0:
      return "0";
    }
  return "UNKNOWN";
}


#if defined (WITH_ENVIRONMENT)
static const char *
config_environment_to_a (int environment)
{
  switch (environment)
    {
    case USER_ENVIRONMENT:
      return "USER_ENVIRONMENT";
    case VIRTUAL_ENVIRONMENT:
      return "VIRTUAL_ENVIRONMENT";
    case OPERATING_ENVIRONMENT:
      return "OPERATING_ENVIRONMENT";
    case 0:
      return "0";
    }
  return "UNKNOWN";
}
#endif


#if defined (WITH_ALIGNMENT)
static const char *
config_alignment_to_a (int alignment)
{
  switch (alignment)
    {
    case NONSTRICT_ALIGNMENT:
      return "NONSTRICT_ALIGNMENT";
    case STRICT_ALIGNMENT:
      return "STRICT_ALIGNMENT";
    case 0:
      return "0";
    }
  return "UNKNOWN";
}
#endif


#if defined (WITH_FLOATING_POINT)
static const char *
config_floating_point_to_a (int floating_point)
{
  switch (floating_point)
    {
    case SOFT_FLOATING_POINT:
      return "SOFT_FLOATING_POINT";
    case HARD_FLOATING_POINT:
      return "HARD_FLOATING_POINT";
    case 0:
      return "0";
    }
  return "UNKNOWN";
}
#endif


void
sim_config (SIM_DESC sd,
	    int prefered_target_byte_order)
{

  /* set the host byte order */
  current_host_byte_order = 1;
  if (*(char*)(&current_host_byte_order))
    current_host_byte_order = LITTLE_ENDIAN;
  else
    current_host_byte_order = BIG_ENDIAN;

  /* verify the host byte order */
  if (CURRENT_HOST_BYTE_ORDER != current_host_byte_order)
    sim_io_error (sd, "host (%s) and configured (%s) byte order in conflict",
		  config_byte_order_to_a (current_host_byte_order),
		  config_byte_order_to_a (CURRENT_HOST_BYTE_ORDER));


  /* set the target byte order */
#if (WITH_DEVICES)
  if (current_target_byte_order == 0)
    current_target_byte_order
      = (tree_find_boolean_property(root, "/options/little-endian?")
	 ? LITTLE_ENDIAN
	 : BIG_ENDIAN);
#endif
  if (current_target_byte_order == 0
      && prefered_target_byte_order != 0)
    current_target_byte_order = prefered_target_byte_order;
  if (current_target_byte_order == 0)
    current_target_byte_order = WITH_TARGET_BYTE_ORDER;

  /* verify the target byte order */
  if (CURRENT_TARGET_BYTE_ORDER == 0)
    sim_io_error (sd, "target byte order unspecified");
  if (CURRENT_TARGET_BYTE_ORDER != current_target_byte_order)
    sim_io_error (sd, "target (%s) and configured (%s) byte order in conflict",
		  config_byte_order_to_a (current_target_byte_order),
		  config_byte_order_to_a (CURRENT_TARGET_BYTE_ORDER));
  if (prefered_target_byte_order != 0
      && CURRENT_TARGET_BYTE_ORDER != prefered_target_byte_order)
    sim_io_error (sd, "target (%s) and specified (%s) byte order in conflict",
		  config_byte_order_to_a (CURRENT_TARGET_BYTE_ORDER),
		  config_byte_order_to_a (prefered_target_byte_order));


  /* set the stdio */
  if (current_stdio == 0)
    current_stdio = WITH_STDIO;
  if (current_stdio == 0)
    current_stdio = DO_USE_STDIO;

  /* verify the stdio */
  if (CURRENT_STDIO == 0)
    sim_io_error (sd, "target standard IO unspecified");
  if (CURRENT_STDIO != current_stdio)
    sim_io_error (sd, "target (%s) and configured (%s) standard IO in conflict",
		  config_stdio_to_a (CURRENT_STDIO),
		  config_stdio_to_a (current_stdio));
		  

#if defined (WITH_ENVIRONMENT)

  /* set the environment */
#if (WITH_DEVICES)
  if (current_environment == 0)
    {
      const char *env =
	tree_find_string_property(root, "/openprom/options/env");
      current_environment = ((strcmp(env, "user") == 0
			      || strcmp(env, "uea") == 0)
			     ? USER_ENVIRONMENT
			     : (strcmp(env, "virtual") == 0
				|| strcmp(env, "vea") == 0)
			     ? VIRTUAL_ENVIRONMENT
			     : (strcmp(env, "operating") == 0
				|| strcmp(env, "oea") == 0)
			     ? OPERATING_ENVIRONMENT
			     : 0);
    }
#endif
  if (current_environment == 0)
    current_environment = WITH_ENVIRONMENT;

  /* verify the environment */
  if (CURRENT_ENVIRONMENT == 0)
    sim_io_error (sd, "target environment unspecified");
  if (CURRENT_ENVIRONMENT != current_environment)
    sim_io_error (sd, "target (%s) and configured (%s) environment in conflict",
		  config_environment_to_a (CURRENT_ENVIRONMENT),
		  config_environment_to_a (current_environment));
#endif


#if defined (WITH_ALIGNMENT)

  /* set the alignment */
#if defined (WITH_DEVICES)
  if (current_alignment == 0)
    current_alignment =
      (tree_find_boolean_property(root, "/openprom/options/strict-alignment?")
       ? STRICT_ALIGNMENT
       : NONSTRICT_ALIGNMENT);
#endif
  if (current_alignment == 0)
    current_alignment = WITH_ALIGNMENT;

  /* verify the alignment */
  if (CURRENT_ALIGNMENT == 0)
    sim_io_error (sd, "target alignment unspecified");
  if (CURRENT_ALIGNMENT != current_alignment)
    sim_io_error (sd, "target (%s) and configured (%s) alignment in conflict",
		  config_alignment_to_a (CURRENT_ALIGNMENT),
		  config_alignment_to_a (current_alignment));
#endif


#if defined (WITH_FLOAING_POINT)

  /* set the floating point */
  if (current_floating_point == 0)
    current_floating_point = WITH_FLOATING_POINT;

  /* verify the floating point */
  if (CURRENT_FLOATING_POINT == 0)
    sim_io_error (sd, "target floating-point unspecified");
  if (CURRENT_FLOATING_POINT != current_floating_point)
    sim_io_error (sd, "target (%s) and configured (%s) floating-point in conflict",
		  config_alignment_to_a (CURRENT_FLOATING_POINT),
		  config_alignment_to_a (current_floating_point));
		  
#endif


}


void
print_sim_config (SIM_DESC sd)
{
#if defined (__GNUC__) && defined (__VERSION__)
  sim_io_printf (sd, "Compiled by GCC %s on %s %s\n",
			  __VERSION__, __DATE__, __TIME__);
#else
  sim_io_printf (sd, "Compiled on %s %s\n", __DATE__, __TIME__);
#endif

  sim_io_printf (sd, "WITH_TARGET_BYTE_ORDER = %s\n",
		 config_byte_order_to_a (WITH_TARGET_BYTE_ORDER));

  sim_io_printf (sd, "WITH_HOST_BYTE_ORDER   = %s\n",
		 config_byte_order_to_a (WITH_HOST_BYTE_ORDER));

  sim_io_printf (sd, "WITH_STDIO             = %s\n",
		 config_stdio_to_a (WITH_STDIO));

#if defined (WITH_XOR_ENDIAN)
  sim_io_printf (sd, "WITH_XOR_ENDIAN = %d\n", WITH_XOR_ENDIAN);
#endif

#if defined (WITH_ENVIRONMENT)
  sim_io_printf (sd, "WITH_ENVIRONMENT = %s\n",
		 config_environment_to_a (WITH_ENVIRONMENT));
#endif

#if defined (WITH_ALIGNMENT)
  sim_io_printf (sd, "WITH_ALIGNMENT = %s\n",
		 config_alignment_to_a (WITH_ALIGNMENT));
#endif

#if defined (WITH_FLOATING_POINT)
  sim_io_printf (sd, "WITH_FLOATING_POINT = %s\n",
		 config_floating_point_to_a (WITH_FLOATING_POINT));
#endif

#if defined (WITH_SMP)
  sim_io_printf (sd, "WITH_SMP = %d\n", WITH_SMP);
#endif

#if defined (WITH_RESERVED_BITS)
  sim_io_printf (sd, "WITH_RESERVED_BITS = %d\n", WITH_RESERVED_BITS);
#endif
		 
}
