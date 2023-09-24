use rand::Rng;
use std::{env, f64::consts::PI};

const VIEWBOX_WIDTH: usize = 400;
const VIEWBOX_HEIGHT: usize = 225;
const VIEWBOX_START: &str = r#"<svg viewBox="0 0 400 225" style="background-color:#000000" xmlns="http://www.w3.org/2000/svg">
"#;
const VIEWBOX_END: &str = "</svg>";

#[derive(Debug)]
struct Star {
    start: (f64, f64),
    end: (f64, f64),
    speed: f64,
    size: f64,
    color: u8,
}

impl Star {
    #[inline]
    fn new<T: Rng>(rng: &mut T, box_h: usize, box_w: usize) -> Star {
        let box_w = box_w as f64;
        let box_h = box_h as f64;

        let mut start: (f64, f64) = (box_w as f64 / 2f64, box_h as f64 / 2f64);

        let mut dir: (f64, f64) = (1f64, 0f64);
        Self::rotate(&mut dir, rng.gen_range(0..360) as f64);

        {
            let step = rng.gen_range(0..20) as f64;
            start.0 += dir.0 * step;
            start.1 += dir.1 * step;
        }

        let x_s = if dir.0 < 0f64 {
            start.0 / -dir.0
        } else {
            (box_w - start.0) / dir.0
        };

        let y_s = if dir.1 < 0f64 {
            start.1 / -dir.1
        } else {
            (box_h - start.1) / dir.1
        };

        let min_s = x_s.min(y_s);

        let end = (dir.0 * min_s, dir.1 * min_s);

        Star {
            start,
            end,
            speed: (rng.gen_range(15..100) as f64) / 10f64,
            size: rng.gen_range(5..20) as f64 / 10f64,
            color: rng.gen_range(240..255),
        }
    }

    #[inline]
    fn rotate(v: &mut (f64, f64), deg: f64) {
        let d = PI * ((deg as f64) / 180f64);
        let x = (d.cos() * v.0) - (d.sin() * v.1);
        let y = (d.sin() * v.0) + (d.cos() * v.1);
        v.0 = x;
        v.1 = y;
    }
}

fn gen_stars(n_stars: usize) -> String {
    let mut rng = rand::thread_rng();
    let mut stars = String::from(VIEWBOX_START);

    for _ in 0..n_stars {
        let star = Star::new(&mut rng, 225, 400);

        let s = format!(
            r#"
  <circle cx="{0:.0}" cy="{1:.0}" r="{2:.2}" style="fill:#{6:02x}{6:02x}{6:02x}">
    <animateTransform
      attributeName="transform"
      attributeType="XML"
      type="translate"
      from="0 0"
      to="{3:.0} {4:.0}"
      dur="{5:.2}"
      repeatCount="indefinite" />
  </circle>
"#,
            star.start.0, star.start.1, star.size, star.end.0, star.end.1, star.speed, star.color
        );

        stars.push_str(s.as_str());
    }

    stars.push_str(VIEWBOX_END);

    stars
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let n_stars = args[1].parse::<usize>().expect("Could not parse n_stars");

    println!("{}", gen_stars(n_stars));
}
