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

struct RenderDataStr;


typedef double basis_3d_fn(double,double,double);
typedef double basis_4d_fn(double,double,double,double);
typedef double basis_5d_fn(double,double,double,double,double);

typedef double basis_fn_type(double,double,double/*,double, double....*/);

/* must be called after the Render Data has been associated to a state */
void InitBasis(struct RenderDataStr *rdat); 

void DeinitBasis();

basis_fn_type *GetBasis();