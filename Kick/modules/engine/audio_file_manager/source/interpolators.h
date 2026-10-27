/*
 * Copyright (c) 2024 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#pragma once

namespace mt
{
    // Interpolators...
    enum
    {
        INTERPOLATOR_LINEAR,
        INTERPOLATOR_HERMITE,
        INTERPOLATOR_CUBIC,
        INTERPOLATOR_SPLINE5,
    };

#define PREFERRED_INTERPOLATOR ( INTERPOLATOR_CUBIC )

    // Linear
    static float linearInterpolator( float frac_pos, float x0, float x1 )
    {
        const float alpha    = frac_pos;
        const float invAlpha = 1.f - alpha;

        return ( ( x0 * invAlpha ) + ( x1 * alpha ) );
    }

    // Hermite
    static float hermiteInterpolator( float frac_pos, float xm1, float x0, float x1, float x2 )
    {
        const float c     = ( x1 - xm1 ) * 0.5f;
        const float v     = x0 - x1;
        const float w     = c + v;
        const float a     = w + v + ( x2 - x0 ) * 0.5f;
        const float b_neg = w + a;

        return ( ( ( ( a * frac_pos ) - b_neg ) * frac_pos + c ) * frac_pos + x0 );
    }

    // Cubic
    static float cubicInterpolator( float frac_pos, float xm1, float x0, float x1, float x2 )
    {
        const float a = ( 3 * ( x0 - x1 ) - xm1 + x2 ) / 2;
        const float b = 2 * x1 + xm1 - ( 5 * x0 + x2 ) / 2;
        const float c = ( x1 - xm1 ) / 2;
        return ( ( ( ( a * frac_pos ) + b ) * frac_pos + c ) * frac_pos + x0 );
    }

    // 5-Point Spline Interpolator
    static double spline5Interpolater( float frac_pos, float xm2, float xm1, float x0, float x1, float x2, float x3 )
    {
        double p0 = xm2;
        double p1 = xm1;
        double p2 = x0;
        double p3 = x1;
        double p4 = x2;
        double p5 = x3;

        return p2 + 0.04166666666 * frac_pos * ( ( p3 - p1 ) * 16.0 + ( p0 - p4 ) * 2.0 + frac_pos * ( ( p3 + p1 ) * 16.0 - p0 - p2 * 30.0 - p4 + frac_pos * ( p3 * 66.0 - p2 * 70.0 - p4 * 33.0 + p1 * 39.0 + p5 * 7.0 - p0 * 9.0 + frac_pos * ( p2 * 126.0 - p3 * 124.0 + p4 * 61.0 - p1 * 64.0 - p5 * 12.0 + p0 * 13.0 + frac_pos * ( ( p3 - p2 ) * 50.0 + ( p1 - p4 ) * 25.0 + ( p5 - p0 ) * 5.0 ) ) ) ) );
    };
}
