use mkb::mkb;

use super::math;

#[derive(Clone, Copy)]
struct Oklab {
    l: f32,
    a: f32,
    b: f32,
}

fn rgb_to_oklab(color: mkb::GXColor) -> Oklab {
    let r = color.r as f32 / 255.0;
    let g = color.g as f32 / 255.0;
    let b = color.b as f32 / 255.0;

    let l = 0.0514459929f32 * b + 0.4122214708f32 * r + 0.5363325363 * g;
    let m = 0.1073969566f32 * b + 0.2119034982f32 * r + 0.6806995451 * g;
    let s = 0.6299787005f32 * b + 0.0883024619f32 * r + 0.2817188376 * g;

    let l_ = math::cbrt_approx(l);
    let m_ = math::cbrt_approx(m);
    let s_ = math::cbrt_approx(s);

    Oklab {
        l: -0.0040720468f32 * s_ + 0.2104542553f32 * l_ + 0.7936177850 * m_,
        a: 0.4505937099f32 * s_ + 1.9779984951f32 * l_ - 2.4285922050 * m_,
        b: -0.8086757660f32 * s_ + 0.0259040371f32 * l_ + 0.7827717662 * m_,
    }
}

fn oklab_to_rgb(color: Oklab) -> mkb::GXColor {
    let l_ = 0.2158037573f32 * color.b + 0.3963377774f32 * color.a + color.l;
    let m_ = -0.0638541728f32 * color.b - 0.1055613458f32 * color.a + color.l;
    let s_ = -1.2914855480f32 * color.b - 0.0894841775f32 * color.a + color.l;

    let l = l_ * l_ * l_;
    let m = m_ * m_ * m_;
    let s = s_ * s_ * s_;

    let r = 0.2309699292f32 * s + 4.0767416621f32 * l - 3.3077115913 * m;
    let g = -0.3413193965f32 * s - 1.2684380046f32 * l + 2.6097574011 * m;
    let b = 1.7076147010f32 * s - 0.0041960863f32 * l - 0.7034186147 * m;

    mkb::GXColor {
        r: (r.clamp(0.0, 1.0) * 255.0) as u8,
        g: (g.clamp(0.0, 1.0) * 255.0) as u8,
        b: (b.clamp(0.0, 1.0) * 255.0) as u8,
        a: 255,
    }
}

pub fn lerp_oklab(t: f32, c1: mkb::GXColor, c2: mkb::GXColor) -> mkb::GXColor {
    let c1_lab = rgb_to_oklab(c1);
    let c2_lab = rgb_to_oklab(c2);

    let result_lab = Oklab {
        l: (1.0 - t) * c1_lab.l + t * c2_lab.l,
        a: (1.0 - t) * c1_lab.a + t * c2_lab.a,
        b: (1.0 - t) * c1_lab.b + t * c2_lab.b,
    };

    oklab_to_rgb(result_lab)
}
