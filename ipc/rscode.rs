use std::io;
use std::io::prelude::*;

fn main() {
    let stdin = io::stdin();
    println!("This is rust code. I received: ");
    for line in stdin.lock().lines() {
        println!("{}", line.unwrap()+"<RUST>");
    }
}