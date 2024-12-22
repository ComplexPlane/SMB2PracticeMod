// Based on: https://crates.io/crates/oklab

#![allow(clippy::excessive_precision)]

use mkb::mkb;

/// A color in Oklab is represented with three coordinates, similar to how CIELAB works, but with better perceptual properties.
///
/// Oklab uses a D65 whitepoint, since this is what sRGB and other common color spaces use.
#[derive(Copy, Clone, Debug, PartialOrd, PartialEq)]
struct Oklab {
    /// L – perceived lightness
    pub l: f32,
    /// a – how green/red the color is
    pub a: f32,
    /// b – how blue/yellow the color is
    pub b: f32,
}

fn linear_srgb_to_oklab(c: mkb::GXColor) -> Oklab {
    let r = c.r as f32 / 255.0;
    let g = c.g as f32 / 255.0;
    let b = c.b as f32 / 255.0;

    let l = 0.0514459929f32 * b + 0.4122214708f32 * r + 0.5363325363 * g;
    let m = 0.1073969566f32 * b + 0.2119034982f32 * r + 0.6806995451 * g;
    let s = 0.6299787005f32 * b + 0.0883024619f32 * r + 0.2817188376 * g;
    let l_ = cbrt(l);
    let m_ = cbrt(m);
    let s_ = cbrt(s);
    Oklab {
        l: -0.0040720468f32 * s_ + 0.2104542553f32 * l_ + 0.7936177850 * m_,
        a: 0.4505937099f32 * s_ + 1.9779984951f32 * l_ - 2.4285922050 * m_,
        b: -0.8086757660f32 * s_ + 0.0259040371f32 * l_ + 0.7827717662 * m_,
    }
}

fn oklab_to_linear_srgb(c: Oklab) -> mkb::GXColor {
    let l_ = 0.2158037573f32 * c.b + 0.3963377774f32 * c.a + c.l;
    let m_ = -0.0638541728f32 * c.b - 0.1055613458f32 * c.a + c.l;
    let s_ = -1.2914855480f32 * c.b - 0.0894841775f32 * c.a + c.l;
    let l = l_ * l_ * l_;
    let m = m_ * m_ * m_;
    let s = s_ * s_ * s_;

    let r = 0.2309699292f32 * s + 4.0767416621f32 * l - 3.3077115913 * m;
    let g = -0.3413193965f32 * s - 1.2684380046f32 * l + 2.6097574011 * m;
    let b = 1.7076147010f32 * s - 0.0041960863f32 * l - 0.7034186147 * m;

    mkb::GXColor {
        r: (r * 255.0) as u8,
        g: (g * 255.0) as u8,
        b: (b * 255.0) as u8,
        a: 0xff,
    }
}

fn cbrt(x: f32) -> f32 {
    const B: u32 = 709957561;
    const C: f32 = 5.4285717010e-1;
    const D: f32 = -7.0530611277e-1;
    const E: f32 = 1.4142856598e+0;
    const F: f32 = 1.6071428061e+0;
    const G: f32 = 3.5714286566e-1;

    let mut t = f32::from_bits((x.to_bits() / 3).wrapping_add(B));
    let s = C + (t * t) * (t / x);
    t *= G + F / (s + E + D / s);
    t
}

pub fn lerp_oklab(t: f32, c1: mkb::GXColor, c2: mkb::GXColor) -> mkb::GXColor {
    let c1_oklab = linear_srgb_to_oklab(c1);
    let c2_oklab = linear_srgb_to_oklab(c2);
    let blended_oklab = Oklab {
        l: (1.0 - t) * c1_oklab.l + t * c2_oklab.l,
        a: (1.0 - t) * c1_oklab.a + t * c2_oklab.a,
        b: (1.0 - t) * c1_oklab.b + t * c2_oklab.b,
    };
    oklab_to_linear_srgb(blended_oklab)
}
