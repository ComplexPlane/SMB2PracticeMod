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

pub fn fmodf(f1: f32, f2: f32) -> f32 {
    let int_result = ((f1 / f2) as i32) as f32;
    f1 - (f2 * int_result)
}

pub fn fmaxf(f1: f32, f2: f32) -> f32 {
    if f1 > f2 {
        f1
    } else {
        f2
    }
}

pub fn fminf(f1: f32, f2: f32) -> f32 {
    if f1 < f2 {
        f1
    } else {
        f2
    }
}
