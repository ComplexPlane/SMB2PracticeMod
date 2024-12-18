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
