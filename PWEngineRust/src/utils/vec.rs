/*
 *
 * 
 */

use std::ops::{Add, Sub, Mul, Div};

trait CalculateAble : Copy + Add<Output=Self> + Sub<Output=Self> + Mul<Output=Self> + Div<Output=Self>{}

impl CalculateAble for i32 {}
impl CalculateAble for f32 {}
impl CalculateAble for u32 {}

pub struct Vec2<T>
{
    pub x : T,
    pub y : T
}

impl<T : CalculateAble> Vec2<T>
{
    fn add(a: Vec2<T>, b: Vec2<T>) -> Vec2<T>
    {
        return Vec2 {x: a.x + b.x, y: a.y + b.y};
    }
    fn minus(a: Vec2<T>, b: Vec2<T>) -> Vec2<T>
    {
        return Vec2 {x: a.x - b.x, y: a.y - b.y};
    }
    fn time(a: T, b : Vec2<T>) -> Vec2<T>
    {
        return Vec2{x: a * b.x, y: a * b.y};
    }
    fn dot(a: Vec2<T>, b: Vec2<T>) -> T
    {
        return a.x * b.x + a.y * b.y;
    }
}