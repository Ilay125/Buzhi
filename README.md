
# 🎨 Drawing Robot

A robotic system that turns digital images into physical drawings — fully standalone.

Combining **embedded systems, robotics, communication protocols, and mobile development**, this project demonstrates end-to-end engineering: from user input to precise real-world motion.

---

## 🚀 Overview

This system enables a user to convert an image into drawing commands via an Android app and transfer it to a robot using **NFC** - no direct connection required.

The robot, powered by a **Raspberry Pi Pico 2**, processes the data and executes accurate motion using synchronized motors to recreate the image on a physical surface.

---

## ⚡ Tech Highlights

- 🧠 **Raspberry Pi Pico 2 (RP2350)** - modern MCU with high performance and dual-core execution  
- 📡 **NFC-based communication** - fully offline, no PC or network required  
- 📺 **SPI-driven display** - real-time feedback and progress indication  
- ⚙️ **Multithreaded firmware** - concurrent motor control + I/O handling  
- 🔌 **USB-C powered standalone system**  

---

## 🧠 System Flow

1. Image selected in Android app  
2. Converted into drawing instructions  
3. Written to NFC card  
4. Robot reads and parses the data  
5. Motion system executes the drawing  

---

## 🦾 Mechanical System

- **5-bar linkage mechanism** for high stability and precision  
- 2 × Stepper motors → planar (XY) motion  
- 1 × Servo motor → pen up/down control  
- Compact and self-contained design (no fixed mounting)

---

## 🗂️ Repository Structure

```bash
/
├── firmware/   # Embedded code (Pico 2)
├── app/        # Android application (image → NFC)
├── simulator/        # Internal simulation (development & debugging)
```

---

### 🔌 firmware/

Embedded code running on the Raspberry Pi Pico 2:

- NFC communication (reading drawing data from cards)  
- Parsing drawing instructions into motion commands  
- Stepper motor control (precise timing & coordination)  
- Servo motor control (pen up/down)  
- Multithreading for smooth and synchronized operation  
- SPI communication with the display (status & progress)  

---

### 📱 app/

Android application responsible for generating the drawing data:

- Image selection and preprocessing  
- Conversion of images into drawing instructions  
- Encoding and writing data to NFC cards  

---

### 🧪 simulator/

Internal simulation and debugging environment:

- Visualization of drawing paths  
- Testing motion logic without hardware  
- Debugging coordinate transformations and control flow  

(Requires a WiFi-enabled board (e.g., Pico W / Pico 2W) for real-time interaction)
