use mkb::mkb;

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

#[derive(Clone, Copy)]
pub struct SinCos {
    pub sin: f32,
    pub cos: f32,
}

pub fn sin_cos(angle: i16) -> SinCos {
    let mut sin_cos = [0f32; 2];
    unsafe {
        mkb::math_sin_cos_v(angle, sin_cos.as_mut_ptr());
    }
    SinCos {
        sin: sin_cos[0],
        cos: sin_cos[1],
    }
}
