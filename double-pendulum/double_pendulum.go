package main

import (
	"image/color"
	"math"
	"time"

	"github.com/faiface/pixel"
	"github.com/faiface/pixel/pixelgl"
)

const (
	width  = 800
	height = 600

	cx = width / 2
	cy = height / 4

	l1 = 200.0
	l2 = 150.0

	m1 = 20.0
	m2 = 20.0

	g  = 9.81
	dt = 0.02
	r  = 10.0
)

func run() {
	cfg := pixelgl.WindowConfig{
		Title:  "Double Pendulum",
		Bounds: pixel.R(0, 0, width, height),
		VSync:  true,
	}

	win, err := pixelgl.NewWindow(cfg)
	if err != nil {
		panic(err)
	}

	theta1 := math.Pi / 2
	theta2 := math.Pi / 2
	omega1 := 0.0
	omega2 := 0.0

	for !win.Closed() {
		// Physics calculations
		delta := theta2 - theta1
		den1 := (m1 + m2) - m2*math.Cos(delta)*math.Cos(delta)
		a1 := (m2*g*math.Sin(theta2)*math.Cos(delta) - m2*math.Sin(delta)*(l1*omega1*omega1*math.Cos(delta)+l2*omega2*omega2) - (m1+m2)*g*math.Sin(theta1)) / (l1 * den1)
		a2 := ((m1+m2)*(l1*omega1*omega1*math.Sin(delta)-g*math.Sin(theta2)+g*math.Sin(theta1)*math.Cos(delta)) + m2*l2*omega2*omega2*math.Sin(delta)*math.Cos(delta)) / (l2 * den1)

		omega1 += a1 * dt
		omega2 += a2 * dt
		theta1 += omega1 * dt
		theta2 += omega2 * dt

		// Positions
		x1 := cx + l1*math.Sin(theta1)
		y1 := cy + l1*math.Cos(theta1)
		x2 := x1 + l2*math.Sin(theta2)
		y2 := y1 + l2*math.Cos(theta2)

		// Clear window
		win.Clear(color.White)

		// Draw rods
		win.SetMatrix(pixel.IM.Moved(pixel.V(0, 0)))
		line := pixel.NewLine(pixel.V(cx, cy), pixel.V(x1, y1))
		line.Draw(win, pixel.IM)
		line2 := pixel.NewLine(pixel.V(x1, y1), pixel.V(x2, y2))
		line2.Draw(win, pixel.IM)

		// Draw masses
		drawCircle(win, x1, y1, r, color.RGBA{255, 0, 0, 255})
		drawCircle(win, x2, y2, r, color.RGBA{255, 0, 0, 255})

		win.Update()
		time.Sleep(time.Millisecond * 16) // ~60 FPS
	}
}

func drawCircle(win *pixelgl.Window, x, y, radius float64, col color.RGBA) {
	steps := 20
	for i := 0; i < steps; i++ {
		angle1 := 2 * math.Pi * float64(i) / float64(steps)
		angle2 := 2 * math.Pi * float64(i+1) / float64(steps)
		x1 := x + radius*math.Cos(angle1)
		y1 := y + radius*math.Sin(angle1)
		x2 := x + radius*math.Cos(angle2)
		y2 := y + radius*math.Sin(angle2)
		pixel.NewLine(pixel.V(x1, y1), pixel.V(x2, y2)).DrawColor(win, col)
	}
}

func main() {
	pixelgl.Run(run)
}
