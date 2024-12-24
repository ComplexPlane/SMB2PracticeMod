use ::mkb::mkb::{self, Vec2d};

pub fn round_up_pow2(n: usize, align: usize) -> usize {
    (n + (align - 1)) & !(align - 1)
}

pub fn round_down_pow2(n: usize, align: usize) -> usize {
    n & !(align - 1)
}

pub fn fabs(n: f32) -> f32 {
    if n < 0.0 {
        -n
    } else {
        n
    }
}

pub fn cbrt_approx(x: f32) -> f32 {
    if x == 0.0 {
        return 0.0;
    }

    let y = x.abs();
    let mut guess = y;

    // Newton's method iterations
    for _ in 0..4 {
        guess = (2.0 * guess + y / (guess * guess)) / 3.0;
    }

    if x < 0.0 {
        -guess
    } else {
        guess
    }
}

pub fn sin_cos(angle: i16) -> Vec2d {
    let mut sin_cos = [0f32; 2];
    unsafe {
        mkb::math_sin_cos_v(angle, sin_cos.as_mut_ptr());
    }
    Vec2d {
        x: sin_cos[0],
        y: sin_cos[1],
    }
}

pub fn map_range(
    value: i32,
    from_range: core::ops::Range<i32>,
    to_range: core::ops::Range<i32>,
) -> i32 {
    let from_span = (from_range.end - from_range.start) as f32;
    let to_span = (to_range.end - to_range.start) as f32;
    let value_scaled = (value - from_range.start) as f32;

    (to_range.start as f32 + (value_scaled * to_span) / from_span) as i32
}
