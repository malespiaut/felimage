/*  Felimage Noise Plugin for the GIMP
 *  Copyright (C) 2005 Guillermo Romero Franco <drirr_gato@users.sourceforge.net>
 *
 *  This file is part of the Felimage Noise Plugin for the GIMP
 *
 *  Felimage Noise Plugin for the Gimp is free software;
 *  you can redistribute it and/or modify it under the terms of
 *  the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  Felimage Noise Plugin for the Gimp is distributed in the hope
 *  that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with fimg-noise; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#pragma once

/* 'A' is the distance squared. This curve drops from 1 at 0 to 0 at 1  with
 * nice 1st, 2nd ant 3rd derivatives at 0 and 1*/
#define KERNEL(A) (1 + (((A - 4.0) * A + 6.0) * A - 4.0) * A)

#define RANDOM(SEED) (1402024253 * SEED + 586950981)
