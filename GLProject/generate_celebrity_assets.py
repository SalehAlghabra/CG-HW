import os
import math
import struct
import zlib

def write_png(filename, width, height, rgba_data):
    """Writes a PNG file with 32-bit RGBA data using only Python standard library."""
    def chunk(tag, data):
        return struct.pack('>I', len(data)) + tag + data + struct.pack('>I', zlib.crc32(tag + data) & 0xffffffff)
    
    raw = bytearray()
    for y in range(height):
        raw.append(0) # Filter type 0 (None)
        offset = y * width * 4
        raw.extend(rgba_data[offset : offset + width * 4])
        
    compressed = zlib.compress(bytes(raw), 9)
    ihdr = struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)
    png_bytes = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', ihdr) + chunk(b'IDAT', compressed) + chunk(b'IEND', b'')
    
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    with open(filename, 'wb') as f:
        f.write(png_bytes)
    print(f"Generated: {filename} ({width}x{height})")

def clamp(val, low=0, high=255):
    return max(low, min(high, int(val)))

def create_card_back():
    """Generates a standard luxury celebrity/VIP card back with golden star emblem & filigree."""
    w, h = 512, 768
    buf = bytearray(w * h * 4)
    cx, cy = w // 2, h // 2
    
    for y in range(h):
        for x in range(w):
            # Rich royal midnight-blue velvet background with radial gradient
            dx = (x - cx) / float(w)
            dy = (y - cy) / float(h)
            dist_center = math.sqrt(dx*dx + dy*dy)
            
            base_r = int(18 * (1.1 - dist_center * 0.7))
            base_g = int(24 * (1.1 - dist_center * 0.7))
            base_b = int(58 * (1.1 - dist_center * 0.7))
            
            # Subtle diamond weave pattern
            weave = ((abs((x - cx) + (y - cy)) // 8) % 2) ^ ((abs((x - cx) - (y - cy)) // 8) % 2)
            if weave:
                base_r += 4
                base_g += 5
                base_b += 12
                
            r, g, b = base_r, base_g, base_b
            
            # Outer Gold Filigree Border
            m1 = 28
            m2 = 42
            if m1 <= x <= w - m1 and m1 <= y <= h - m1:
                # Outer solid gold line
                if x < m1 + 5 or x > w - m1 - 6 or y < m1 + 5 or y > h - m1 - 6:
                    r, g, b = 225, 185, 55
                # Inner thin gold line
                elif m2 <= x <= w - m2 and m2 <= y <= h - m2:
                    if x < m2 + 3 or x > w - m2 - 4 or y < m2 + 3 or y > h - m2 - 4:
                        r, g, b = 185, 145, 40
                        
            # Corner ornamental flourishes
            for corner_x, corner_y in [(m1+20, m1+20), (w-m1-20, m1+20), (m1+20, h-m1-20), (w-m1-20, h-m1-20)]:
                cdist = math.sqrt((x - corner_x)**2 + (y - corner_y)**2)
                if 12 <= cdist <= 16:
                    r, g, b = 215, 175, 50
                elif cdist < 6:
                    r, g, b = 240, 200, 60
            
            # Central VIP / Hollywood Star Medallion
            cdx = x - cx
            cdy = (y - cy) * 0.85 # slightly adjusted aspect
            sdist = math.sqrt(cdx*cdx + cdy*cdy)
            angle = math.atan2(cdy, cdx)
            
            # Radiating gold rays
            if 80 <= sdist <= 150:
                rays = math.cos(angle * 16.0) ** 4
                fade = (1.0 - abs(sdist - 115) / 35.0)
                if rays > 0.1 and fade > 0:
                    r = clamp(r + rays * fade * 160)
                    g = clamp(g + rays * fade * 130)
                    b = clamp(b + rays * fade * 40)
                    
            # Concentric ornate rings
            if 112 <= sdist <= 118 or 78 <= sdist <= 84:
                r, g, b = 220, 180, 50
                
            # Central 8-pointed Hollywood Golden Star
            if sdist < 80:
                # 8-point star formula
                star_r = 72.0 * (0.45 + 0.55 * (max(0.0, math.cos(angle * 4.0))**3 + max(0.0, math.cos(angle * 4.0 + math.pi))**3 * 0.6))
                if sdist < star_r:
                    # Shaded facets of the 3D gold star
                    facet = math.sin(angle * 8.0)
                    if facet > 0:
                        r = clamp(235 + facet * 20)
                        g = clamp(195 + facet * 25)
                        b = clamp(70 + facet * 30)
                    else:
                        r = clamp(180 + facet * 40)
                        g = clamp(140 + facet * 35)
                        b = clamp(40 + facet * 20)
                elif sdist < star_r + 4:
                    r, g, b = 140, 105, 25
                    
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/card_back.png", w, h, buf)

def create_celebrity_wallpaper():
    """Generates a luxury Art Deco Hollywood VIP Lounge wallpaper with fine gold motifs."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    
    for y in range(h):
        for x in range(w):
            # Rich dark charcoal-navy luxury velvet base
            base_r = 20 + int(3 * math.sin(x * 0.05))
            base_g = 22 + int(3 * math.cos(y * 0.05))
            base_b = 30 + int(4 * math.sin((x+y)*0.03))
            
            # Repeating Art Deco Fan / Chevrons
            cell_w = 64
            cell_h = 64
            cx = x % cell_w
            cy = y % cell_h
            
            # Diagonal interlocking gold lattice lines
            diag1 = abs(cx - cy)
            diag2 = abs((cell_w - cx) - cy)
            
            is_gold = False
            gold_intensity = 0.0
            
            if diag1 in (0, 1) or diag2 in (0, 1):
                gold_intensity = 0.75
            elif (cx == 0 or cy == 0) and (x % 128 < 64):
                gold_intensity = 0.5
                
            # Golden diamond medallion in center of each cell
            dcx = cx - cell_w // 2
            dcy = cy - cell_h // 2
            d_dist = abs(dcx) + abs(dcy)
            if 14 <= d_dist <= 16:
                gold_intensity = max(gold_intensity, 0.9)
            elif d_dist < 4:
                gold_intensity = max(gold_intensity, 1.0)
                
            r = clamp(base_r + gold_intensity * 195)
            g = clamp(base_g + gold_intensity * 155)
            b = clamp(base_b + gold_intensity * 45)
            
            idx = (y * w + x) * 4
            buf[idx] = r
            buf[idx+1] = g
            buf[idx+2] = b
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/wallpaper.png", w, h, buf)

def create_celebrity_floor():
    """Generates an espresso dark polished hardwood floor with warm luxury sheen."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    plank_h = 64
    plank_w = 256
    
    for y in range(h):
        row = y // plank_h
        y_in_plank = y % plank_h
        for x in range(w):
            shift = (row * 137) % plank_w
            col = (x + shift) // plank_w
            x_in_plank = (x + shift) % plank_w
            
            # Rich dark espresso hardwood tones
            base_r = 45 + (row * 7 + col * 11) % 12
            base_g = 28 + (row * 5 + col * 9) % 8
            base_b = 20 + (row * 4 + col * 7) % 6
            
            # Wood grain stripes
            grain = math.sin((x + shift) * 0.12 + math.sin(y * 0.18) * 1.5) * 8.0
            r = base_r + grain
            g = base_g + grain * 0.6
            b = base_b + grain * 0.4
            
            # Deep dark beveled plank groove
            if y_in_plank < 2 or y_in_plank > plank_h - 3 or x_in_plank < 2 or x_in_plank > plank_w - 3:
                r *= 0.35
                g *= 0.35
                b *= 0.35
                
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/floor_wood.png", w, h, buf)

def create_celebrity_table():
    """Generates a luxury VIP celebrity card tabletop with dark mahogany and gold card zones."""
    w, h = 1024, 1024
    buf = bytearray(w * h * 4)
    
    for y in range(h):
        for x in range(w):
            # Rich dark polished mahogany wood rim
            grain = math.sin(y * 0.05 + math.cos(x * 0.015) * 4.0) * 10.0
            r = 55 + grain
            g = 28 + grain * 0.5
            b = 18 + grain * 0.3
            
            # Luxurious emerald-black velvet gaming felt
            if 70 <= x <= w - 70 and 70 <= y <= h - 70:
                weave = ((x ^ y) & 4) * 1.2
                r = 18 + weave
                g = 36 + weave
                b = 28 + weave
                
                # Double gold border around felt
                if x in (70, 71, 72, w-73, w-72, w-71) or y in (70, 71, 72, h-73, h-72, h-71):
                    r, g, b = 220, 185, 55
                elif x in (78, w-79) or y in (78, h-79):
                    r, g, b = 175, 140, 40
                    
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255

    def draw_zone(x1, y1, x2, y2, color, border_width=3, fill_color=None):
        for y in range(y1, y2):
            for x in range(x1, x2):
                is_border = (x < x1 + border_width or x >= x2 - border_width or
                             y < y1 + border_width or y >= y2 - border_width)
                idx = (y * w + x) * 4
                if is_border:
                    buf[idx] = color[0]
                    buf[idx+1] = color[1]
                    buf[idx+2] = color[2]
                elif fill_color:
                    buf[idx] = fill_color[0]
                    buf[idx+1] = fill_color[1]
                    buf[idx+2] = fill_color[2]

    # Section 1: PLAYER DECK ZONE (Left side near player)
    draw_zone(115, 145, 275, 455, (230, 195, 65), 4, (15, 30, 22))
    draw_zone(128, 158, 262, 442, (185, 150, 45), 2)

    # Section 2: PLAYER PLAYED CARDS SLOTS (5 slots in center near player)
    slot_w = 108
    slot_gap = 18
    start_x = 320
    for i in range(5):
        sx1 = start_x + i * (slot_w + slot_gap)
        sx2 = sx1 + slot_w
        draw_zone(sx1, 145, sx2, 455, (225, 185, 55), 3, (20, 38, 28))
        draw_zone(sx1 + 8, 153, sx2 - 8, 447, (170, 135, 35), 1)

    # Dividing central gold line with VIP star crest
    for x in range(95, w - 95):
        for y in range(509, 515):
            idx = (y * w + x) * 4
            buf[idx] = 200
            buf[idx+1] = 165
            buf[idx+2] = 50

    # Section 3: OPPONENT CARDS ZONE (Empty 5 slots)
    for i in range(5):
        sx1 = start_x + i * (slot_w + slot_gap)
        sx2 = sx1 + slot_w
        draw_zone(sx1, 565, sx2, 875, (160, 65, 65), 2, (28, 20, 22))
    draw_zone(115, 565, 275, 875, (150, 60, 60), 2, (25, 18, 20))

    write_png("GLProject/assets/textures/table_wood.png", w, h, buf)

def create_celebrity_poster():
    """Generates a glamorous Hollywood Premiere / Cannes Gala Poster texture."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    cx, cy = w // 2, h // 2
    
    for y in range(h):
        for x in range(w):
            # Midnight blue/black background with intersecting spotlights
            r, g, b = 15, 18, 28
            
            # Left spotlight beam angled across
            spot1_dx = x - 90
            spot1_dy = y - 30
            angle1 = math.atan2(spot1_dy, spot1_dx)
            if 0.6 < angle1 < 1.4:
                beam1 = math.sin((angle1 - 0.6) / 0.8 * math.pi) ** 2 * 0.45
                r += int(beam1 * 140)
                g += int(beam1 * 170)
                b += int(beam1 * 255)
                
            # Right spotlight beam
            spot2_dx = x - 420
            spot2_dy = y - 30
            angle2 = math.atan2(spot2_dy, -spot2_dx)
            if 0.6 < angle2 < 1.4:
                beam2 = math.sin((angle2 - 0.6) / 0.8 * math.pi) ** 2 * 0.45
                r += int(beam2 * 255)
                g += int(beam2 * 210)
                b += int(beam2 * 140)
                
            # Ornate gold frame
            if x < 20 or x > w - 21 or y < 20 or y > h - 21:
                r, g, b = 210, 175, 50
            elif x in (26, w-27) or y in (26, h-27):
                r, g, b = 160, 130, 35
                
            # Central Golden Walk of Fame Star / Award Crest
            dx = x - cx
            dy = y - cy
            dist = math.sqrt(dx*dx + dy*dy)
            ang = math.atan2(dy, dx)
            
            if dist < 120:
                # 5-pointed Hollywood Star
                star_val = 110.0 * (0.42 + 0.58 * (max(0.0, math.cos(ang * 2.5)) ** 3 + max(0.0, math.cos(ang * 2.5 + math.pi)) ** 3 * 0.6))
                if dist < star_val:
                    r = clamp(235 + math.cos(ang * 5.0) * 20)
                    g = clamp(190 + math.cos(ang * 5.0) * 25)
                    b = clamp(60 + math.cos(ang * 5.0) * 20)
                    # Camera symbol inside star
                    if dist < 32 and (abs(dx) < 22 and abs(dy) < 18):
                        r, g, b = 40, 35, 45
                        
            # Golden Laurels curve around star
            if 125 <= dist <= 145:
                leaf = abs(math.sin(ang * 12.0)) > 0.35
                if leaf and (abs(dx) > 40):
                    r = clamp(r + 140)
                    g = clamp(g + 115)
                    b = clamp(b + 30)
                    
            # Top Banner: "HOLLYWOOD GALA"
            if y > h - 85 and y < h - 45 and 60 < x < w - 60:
                r = clamp(r * 0.4 + 40)
                g = clamp(g * 0.4 + 35)
                b = clamp(b * 0.4 + 50)
                if y in (h - 84, h - 46):
                    r, g, b = 220, 185, 55
                    
            # Bottom Banner: "RED CARPET PREMIERE"
            if y > 45 and y < 85 and 60 < x < w - 60:
                r = clamp(r * 0.4 + 40)
                g = clamp(g * 0.4 + 35)
                b = clamp(b * 0.4 + 50)
                if y in (46, 84):
                    r, g, b = 220, 185, 55

            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/poster_starry_night.png", w, h, buf)
    write_png("GLProject/assets/textures/poster_celebrity.png", w, h, buf)

def create_celebrity_showcase():
    """Generates a luxury Celebrity Showcase Cabinet texture with Platinum/Gold records and awards."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    
    num_shelves = 3
    shelf_h = h // num_shelves
    
    for y in range(h):
        shelf_idx = y // shelf_h
        y_in_shelf = y % shelf_h
        for x in range(w):
            # Outer mahogany and gold frame
            if x < 18 or x > w - 19:
                r, g, b = 75, 42, 22
            elif y_in_shelf < 16 or y_in_shelf > shelf_h - 8:
                r, g, b = 85, 48, 25
                if y_in_shelf in (14, 15, shelf_h - 7):
                    r, g, b = 215, 175, 50 # Gold shelf trim
            else:
                # Dark luxury mirrored backing
                r, g, b = 22, 24, 32
                
                # Shelf 0 (Top Shelf): Platinum Record & Golden Statuette
                if shelf_idx == 2:
                    # Platinum Record (Left side)
                    rec_cx, rec_cy = 150, 85
                    rdist = math.sqrt((x - rec_cx)**2 + (y_in_shelf - rec_cy)**2)
                    if rdist < 55:
                        grooves = math.sin(rdist * 0.8) * 20
                        r = clamp(195 + grooves)
                        g = clamp(200 + grooves)
                        b = clamp(215 + grooves)
                        if rdist < 18:
                            r, g, b = 40, 90, 180 # Blue record center label
                        elif rdist < 4:
                            r, g, b = 15, 15, 20
                    # Oscar statuette plaque (Right side)
                    elif 300 < x < 420 and 30 < y_in_shelf < 135:
                        r, g, b = 215, 180, 55 # Gold statuette silhouette
                        
                # Shelf 1 (Middle Shelf): Gold Record & Walk of Fame Star Plaque
                elif shelf_idx == 1:
                    # Gold Record (Right side)
                    rec_cx, rec_cy = 360, 85
                    rdist = math.sqrt((x - rec_cx)**2 + (y_in_shelf - rec_cy)**2)
                    if rdist < 55:
                        grooves = math.sin(rdist * 0.8) * 20
                        r = clamp(225 + grooves)
                        g = clamp(185 + grooves * 0.8)
                        b = clamp(55 + grooves * 0.3)
                        if rdist < 18:
                            r, g, b = 180, 30, 40 # Crimson record center label
                        elif rdist < 4:
                            r, g, b = 15, 15, 20
                    # Star Plaque (Left side)
                    elif 100 < x < 210 and 35 < y_in_shelf < 135:
                        r, g, b = 210, 170, 50
                        
                # Shelf 2 (Bottom Shelf): Luxury Crystal Decanters & VIP Awards
                elif shelf_idx == 0:
                    for bottle_x in (110, 200, 310, 400):
                        if abs(x - bottle_x) < 22 and 20 < y_in_shelf < 125:
                            r, g, b = 205, 175, 80 # Golden champagne / amber cognac

            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/bookshelf.png", w, h, buf)
    write_png("GLProject/assets/textures/showcase_celebrity.png", w, h, buf)

def create_celebrity_star_crest():
    """Generates the Hollywood Walk of Fame Star Crest texture for Decor 3."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    cx, cy = w // 2, h // 2
    
    for y in range(h):
        for x in range(w):
            dx = x - cx
            dy = y - cy
            dist = math.sqrt(dx*dx + dy*dy)
            ang = math.atan2(dy, dx)
            
            # Transparent outside
            r, g, b, a = 0, 0, 0, 0
            
            # Outer circular golden halo
            if 180 <= dist <= 210:
                halo = math.sin((dist - 180) / 30.0 * math.pi)
                r = clamp(225 * halo)
                g = clamp(185 * halo)
                b = clamp(55 * halo)
                a = clamp(255 * halo)
            # Five-pointed Hollywood Star
            elif dist < 180:
                star_r = 175.0 * (0.42 + 0.58 * (max(0.0, math.cos(ang * 2.5)) ** 3 + max(0.0, math.cos(ang * 2.5 + math.pi)) ** 3 * 0.6))
                if dist < star_r:
                    # Coral pink terrazzo inside star
                    r, g, b, a = 235, 110, 110, 255
                    # Gold border of star
                    if dist > star_r - 8:
                        r, g, b = 240, 200, 60
                    # Center Movie Camera emblem
                    elif dist < 45:
                        r, g, b = 230, 195, 60
                elif dist < star_r + 6:
                    r, g, b, a = 210, 170, 50, 255
                    
            idx = (y * w + x) * 4
            buf[idx] = r
            buf[idx+1] = g
            buf[idx+2] = b
            buf[idx+3] = a
            
    write_png("GLProject/assets/textures/shield.png", w, h, buf)
    write_png("GLProject/assets/textures/celebrity_star.png", w, h, buf)

def create_oscar_3d_model():
    """Generates a clean Wavefront .OBJ 3D model of an Oscar / Academy Award Statuette."""
    # Profile points: (radius, y)
    profile = [
        (0.00, 0.00),    # Base bottom center
        (0.42, 0.00),    # Base rim bottom
        (0.40, 0.08),    # Base lower step
        (0.35, 0.12),    # Base mid bevel
        (0.35, 0.22),    # Base vertical cylinder
        (0.30, 0.28),    # Film reel lower taper
        (0.30, 0.42),    # Film reel top
        (0.24, 0.44),    # Statuette feet platform
        (0.12, 0.52),    # Ankles
        (0.15, 0.75),    # Calves / knees
        (0.16, 0.95),    # Thighs
        (0.14, 1.10),    # Waist
        (0.20, 1.35),    # Chest / Torso
        (0.24, 1.48),    # Shoulders
        (0.08, 1.54),    # Neck
        (0.11, 1.62),    # Head mid
        (0.09, 1.72),    # Head crown
        (0.00, 1.74)     # Head top center
    ]
    
    segments = 24
    vertices = []
    normals = []
    uvs = []
    faces = []
    
    for p_idx, (r, y) in enumerate(profile):
        v_coord = y / 1.74
        for s in range(segments):
            angle = (2.0 * math.pi * s) / segments
            x = r * math.cos(angle)
            z = r * math.sin(angle)
            u_coord = s / float(segments)
            
            if r > 0.001:
                nx = math.cos(angle)
                nz = math.sin(angle)
                ny = 0.1
            else:
                nx, ny, nz = 0.0, 1.0, 0.0
            mag = math.sqrt(nx*nx + ny*ny + nz*nz)
            nx, ny, nz = nx/mag, ny/mag, nz/mag
            
            vertices.append((x, y, z))
            normals.append((nx, ny, nz))
            uvs.append((u_coord, v_coord))
            
    for p in range(len(profile) - 1):
        for s in range(segments):
            next_s = (s + 1) % segments
            
            i1 = p * segments + s + 1
            i2 = p * segments + next_s + 1
            i3 = (p + 1) * segments + next_s + 1
            i4 = (p + 1) * segments + s + 1
            
            faces.append((i1, i2, i3, i4))
            
    # Write OBJ to both oscar_trophy.obj and chalice.obj for seamless compatibility
    for obj_name in ["oscar_trophy.obj", "chalice.obj"]:
        obj_path = f"GLProject/assets/models/{obj_name}"
        with open(obj_path, "w") as f:
            f.write("# Wavefront OBJ: Academy Award Oscar 3D Model\n")
            f.write("mtllib chalice.mtl\n")
            f.write("o OscarTrophy\n")
            for v in vertices:
                f.write(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
            for vt in uvs:
                f.write(f"vt {vt[0]:.4f} {vt[1]:.4f}\n")
            for vn in normals:
                f.write(f"vn {vn[0]:.4f} {vn[1]:.4f} {vn[2]:.4f}\n")
            f.write("usemtl GoldMaterial\n")
            f.write("s 1\n")
            for face in faces:
                f.write(f"f {face[0]}/{face[0]}/{face[0]} {face[1]}/{face[1]}/{face[1]} {face[2]}/{face[2]}/{face[2]} {face[3]}/{face[3]}/{face[3]}\n")
        print(f"Generated 3D Model: {obj_path}")

def main():
    print("=== Generating High-Quality Celebrity-Themed Assets ===")
    create_card_back()
    create_celebrity_wallpaper()
    create_celebrity_floor()
    create_celebrity_table()
    create_celebrity_poster()
    create_celebrity_showcase()
    create_celebrity_star_crest()
    create_oscar_3d_model()
    print("=== All Celebrity Assets Generated Successfully! ===")

if __name__ == "__main__":
    main()
