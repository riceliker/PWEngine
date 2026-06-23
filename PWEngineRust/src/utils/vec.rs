/*
 *
 * 
 */

use std::{ops::{Add, Div, Mul, Sub}, ptr};

use sdl3_sys::pixels::*;

trait CalculateAble : Copy + Add<Output=Self> + Sub<Output=Self> + Mul<Output=Self> + Div<Output=Self>{}

impl CalculateAble for i32 {}
impl CalculateAble for f32 {}
impl CalculateAble for u32 {}

pub struct Vec2<T>
{
    pub x: T,
    pub y: T
}

impl<T : CalculateAble> Vec2<T>
{
    pub fn new(x: T, y: T) -> Self
    {
        return Self { x: x, y: y };
    }

    pub fn add(a: Vec2<T>, b: Vec2<T>) -> Vec2<T>
    {
        return Vec2 {x: a.x + b.x, y: a.y + b.y};
    }
    pub fn minus(a: Vec2<T>, b: Vec2<T>) -> Vec2<T>
    {
        return Vec2 {x: a.x - b.x, y: a.y - b.y};
    }
    pub fn time(a: T, b: Vec2<T>) -> Vec2<T>
    {
        return Vec2{x: a * b.x, y: a * b.y};
    }
    pub fn dot(a: Vec2<T>, b: Vec2<T>) -> T
    {
        return a.x * b.x + a.y * b.y;
    }
}

pub struct PWEColor
{
    pub r: u8, pub g: u8, pub b: u8, pub a: u8
}
impl PWEColor 
{
    pub fn for_sdl3(&self) -> SDL_Color
    {
        return SDL_Color { r: self.r, g: self.g, b: self.b, a: self.a}
    }
}
