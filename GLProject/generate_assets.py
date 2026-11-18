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

def create_wood_floor():
    """Generates a parquet / plank wooden floor texture."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    plank_h = 64
    plank_w = 256
    
    for y in range(h):
        row_idx = y // plank_h
        y_in_plank = y % plank_h
        for x in range(w):
            shift = (row_idx * 137) % plank_w
            col_idx = (x + shift) // plank_w
            x_in_plank = (x + shift) % plank_w
            
            # Base wood tone with row variation
            base_r = 135 + (row_idx * 17 + col_idx * 23) % 25
            base_g = 85 + (row_idx * 13 + col_idx * 19) % 20
            base_b = 45 + (row_idx * 7 + col_idx * 11) % 15
            
            # Wood grain stripes
            grain = math.sin((x + shift) * 0.08 + math.sin(y * 0.15) * 2.0) * 12.0
            fine_grain = math.sin((x + shift) * 0.4 + y * 0.02) * 6.0
            
            r = base_r + grain + fine_grain
            g = base_g + grain * 0.7 + fine_grain * 0.7
            b = base_b + grain * 0.4 + fine_grain * 0.4
            
            # Plank border bevel / grooves
            if y_in_plank < 2 or y_in_plank > plank_h - 3 or x_in_plank < 2 or x_in_plank > plank_w - 3:
                r *= 0.4
                g *= 0.4
                b *= 0.4
            
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/floor_wood.png", w, h, buf)

def create_wallpaper():
    """Generates an elegant Victorian damask wallpaper texture."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    
    for y in range(h):
        for x in range(w):
            # Rich navy / deep midnight-blue base
            base_r = 25 + int(5 * math.sin(x * 0.03))
            base_g = 35 + int(5 * math.cos(y * 0.03))
            base_b = 65 + int(10 * math.sin((x+y)*0.02))
            
            # Repeating damask floral medallion
            cell_x = (x % 128) - 64
            cell_y = (y % 128) - 64
            dist = math.sqrt(cell_x*cell_x + cell_y*cell_y)
            angle = math.atan2(cell_y, cell_x)
            
            # Petals and floral pattern
            petals = math.cos(angle * 6.0)
            pattern = math.exp(-((dist - 35)**2) / 100.0) * (0.6 + 0.4 * petals)
            center_dot = math.exp(-(dist**2) / 80.0)
            corner_dots = math.exp(-(((cell_x - 32)**2 + (cell_y - 32)**2)) / 50.0)
            
            damask = max(0.0, pattern * 1.2 + center_dot * 1.5 + corner_dots * 0.8)
            
            # Gold / amber damask embroidery highlights
            r = base_r + damask * 140
            g = base_g + damask * 110
            b = base_b + damask * 40
            
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/wallpaper.png", w, h, buf)

def create_table_texture():
    """Generates a mahogany card game tabletop texture with clearly demarcated sections."""
    w, h = 1024, 1024
    buf = bytearray(w * h * 4)
    
    for y in range(h):
        for x in range(w):
            # Base dark mahogany wood
            grain = math.sin(y * 0.06 + math.cos(x * 0.015) * 4.0) * 15.0
            r = 75 + grain
            g = 40 + grain * 0.5
            b = 25 + grain * 0.3
            
            # Subtle green felt center inlay for gaming area
            if 80 <= x <= w - 80 and 80 <= y <= h - 80:
                # Felt felt weave texture
                weave = ((x ^ y) & 4) * 1.5
                felt_r = 28 + weave
                felt_g = 58 + weave
                felt_b = 38 + weave
                r, g, b = felt_r, felt_g, felt_b
                
                # Outer gold decorative border of felt area
                if x in (80, 81, 82, w-83, w-82, w-81) or y in (80, 81, 82, h-83, h-82, h-81):
                    r, g, b = 210, 175, 55
            
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255

    # Helper to draw a framed rectangle zone on the table
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
    # y: 150 to 450 (player side), x: 120 to 280
    draw_zone(120, 150, 280, 450, (220, 185, 60), 4, (20, 45, 30))
    # Inner ornament for Deck
    draw_zone(135, 165, 265, 435, (180, 145, 40), 2)

    # Section 2: PLAYER PLAYED CARDS SLOTS (5 slots in center near player)
    # y: 150 to 450, 5 slots across x: 320 to 920
    slot_w = 105
    slot_gap = 18
    start_x = 320
    for i in range(5):
        sx1 = start_x + i * (slot_w + slot_gap)
        sx2 = sx1 + slot_w
        draw_zone(sx1, 150, sx2, 450, (215, 175, 50), 3, (25, 50, 35))
        draw_zone(sx1 + 8, 158, sx2 - 8, 442, (160, 130, 30), 1)

    # Dividing central line between Player and Opponent
    for x in range(100, w - 100):
        for y in range(508, 514):
            idx = (y * w + x) * 4
            buf[idx] = 180
            buf[idx+1] = 145
            buf[idx+2] = 45

    # Section 3: OPPONENT CARDS ZONE (Empty 5 slots on top half: y: 570 to 870)
    for i in range(5):
        sx1 = start_x + i * (slot_w + slot_gap)
        sx2 = sx1 + slot_w
        # Subtle silver/crimson border indicating opponent's slots (empty)
        draw_zone(sx1, 570, sx2, 870, (160, 70, 70), 2, (35, 25, 28))
    # Opponent Deck zone (symmetric on opponent side)
    draw_zone(120, 570, 280, 870, (150, 60, 60), 2, (30, 22, 25))

    write_png("GLProject/assets/textures/table_wood.png", w, h, buf)

def create_card_back():
    """Generates an ornate fantasy card back with Celtic knotwork and glowing seal."""
    w, h = 512, 768
    buf = bytearray(w * h * 4)
    cx, cy = w // 2, h // 2
    
    for y in range(h):
        for x in range(w):
            # Rich deep crimson/burgundy leather background
            leather = ((x * 17 + y * 23) % 11) * 1.5
            r = 70 + leather
            g = 15 + leather * 0.4
            b = 25 + leather * 0.5
            
            # Gold filigree border
            margin = 32
            inner_margin = 48
            if (margin <= x <= w - margin and margin <= y <= h - margin):
                if x < margin + 6 or x > w - margin - 7 or y < margin + 6 or y > h - margin - 7:
                    r, g, b = 220, 180, 55
                elif (inner_margin <= x <= w - inner_margin and inner_margin <= y <= h - inner_margin):
                    if x < inner_margin + 3 or x > w - inner_margin - 4 or y < inner_margin + 3 or y > h - inner_margin - 4:
                        r, g, b = 180, 140, 35
            
            # Central ornate seal
            dx = x - cx
            dy = (y - cy) * 0.8
            dist = math.sqrt(dx*dx + dy*dy)
            
            if 80 <= dist <= 120:
                ring = math.sin(dist * 0.3) * 0.5 + 0.5
                angle = math.atan2(dy, dx)
                rot_star = math.cos(angle * 8.0) * 0.5 + 0.5
                r = clamp(r + ring * rot_star * 160)
                g = clamp(g + ring * rot_star * 130)
                b = clamp(b + ring * rot_star * 40)
            elif dist < 80:
                # Glowing blue core
                glow = max(0.0, 1.0 - dist / 80.0)
                r = clamp(r + glow * 50)
                g = clamp(g + glow * 120)
                b = clamp(b + glow * 220)
                # Star emblem
                angle = math.atan2(dy, dx)
                star = max(0.0, math.cos(angle * 4.0)) ** 4
                if star > 0.3 and dist < 65:
                    r, g, b = 240, 240, 255
            
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/card_back.png", w, h, buf)

def create_card_front(name, main_color, accent_color, symbol_type):
    """
    Generates a card front texture for a specific fictional character:
    - Geralt of Rivia (White Wolf / Witcher runes)
    - Gandalf (Grey/Starry magic staff & wizard hat)
    - Batman (Dark Bat silhouette & Bat-Signal)
    - Spider-Man (Red/Blue web motif & spider crest)
    - Son Goku (Dragon Ball 4-star / Super Saiyan golden aura)
    """
    w, h = 512, 768
    buf = bytearray(w * h * 4)
    cx, cy = w // 2, h // 2 + 30
    
    for y in range(h):
        for x in range(w):
            # Outer dark metallic frame
            r, g, b = 25, 25, 30
            
            # Gold card border
            if 20 <= x <= w - 20 and 20 <= y <= h - 20:
                if x < 28 or x > w - 29 or y < 28 or y > h - 29:
                    r, g, b = 215, 175, 50
                elif 36 <= x <= w - 36 and 36 <= y <= h - 36:
                    if x < 40 or x > w - 41 or y < 40 or y > h - 41:
                        r, g, b = 170, 130, 35
                    else:
                        # Inner portrait window background
                        dx = (x - cx) / 180.0
                        dy = (y - cy) / 220.0
                        grad = math.sqrt(dx*dx + dy*dy)
                        # Main theme gradient
                        r = clamp(main_color[0] * (1.2 - grad * 0.6) + accent_color[0] * 0.2)
                        g = clamp(main_color[1] * (1.2 - grad * 0.6) + accent_color[1] * 0.2)
                        b = clamp(main_color[2] * (1.2 - grad * 0.6) + accent_color[2] * 0.2)
                        
                        # Top Header Banner area (Character Name plate)
                        if y > h - 120 and y < h - 45:
                            r, g, b = 35, 30, 45
                            if y == h - 119 or y == h - 46:
                                r, g, b = 210, 175, 45
                        
                        # Bottom Stats Banner area (Attack / Defense gems)
                        if y > 45 and y < 115:
                            r, g, b = 35, 30, 45
                            if y == 46 or y == 114:
                                r, g, b = 210, 175, 45
                                
            # Character specific artistic iconography
            rel_x = x - cx
            rel_y = y - cy
            dist = math.sqrt(rel_x*rel_x + rel_y*rel_y)
            
            if symbol_type == "geralt":
                # Witcher Wolf Head / Medallion angular silhouette
                # Eyes glowing amber
                eye_l = (rel_x + 35)**2 + (rel_y - 25)**2 < 80
                eye_r = (rel_x - 35)**2 + (rel_y - 25)**2 < 80
                # Wolf muzzle and ears
                wolf_snout = abs(rel_x) < (rel_y + 80) * 0.5 and -80 < rel_y < 40
                ears = (abs(rel_x) - 70)**2 + (rel_y - 90)**2 < 700 and rel_y > 40
                if eye_l or eye_r:
                    r, g, b = 255, 200, 20 # Glowing amber cat eyes
                elif wolf_snout or ears:
                    r, g, b = 225, 230, 240 # Silver white wolf
                elif 70 <= dist <= 140:
                    # Witcher runic circle
                    r = clamp(r + 40)
                    g = clamp(g + 30)
                    b = clamp(b + 60)
                    
            elif symbol_type == "gandalf":
                # Radiant white/silver Wizard Staff & Star of Earendil
                # Vertical crystal staff beam
                if abs(rel_x) < 8 and -120 < rel_y < 120:
                    r, g, b = 220, 220, 255
                # Glowing magic crystal star
                star_dist = math.sqrt(rel_x*rel_x + (rel_y - 90)**2)
                if star_dist < 45:
                    magic = max(0.0, 1.0 - star_dist / 45.0)
                    r = clamp(r + magic * 200)
                    g = clamp(g + magic * 220)
                    b = clamp(b + magic * 255)
                # Pointed Hat silhouette
                hat_cone = rel_y > -40 and rel_y < 80 and abs(rel_x) < (80 - rel_y) * 0.6
                hat_brim = abs(rel_y + 40) < 8 and abs(rel_x) < 85
                if hat_cone or hat_brim:
                    r, g, b = 75, 80, 95 # Gandalf grey cloak
                    
            elif symbol_type == "batman":
                # Batman iconic bat emblem
                # Wings
                nx = rel_x / 130.0
                ny = rel_y / 70.0
                if nx*nx + ny*ny < 1.0:
                    # Bat silhouette carve-out
                    is_bat = True
                    # Wing scalloping
                    if ny < -0.2:
                        cut1 = (rel_x + 60)**2 + (rel_y + 40)**2 < 1200
                        cut2 = (rel_x - 60)**2 + (rel_y + 40)**2 < 1200
                        cut3 = rel_x**2 + (rel_y + 50)**2 < 800
                        if cut1 or cut2 or cut3:
                            is_bat = False
                    # Bat ears
                    if rel_y > 45 and abs(rel_x) > 18:
                        is_bat = False
                    if is_bat:
                        r, g, b = 15, 18, 22 # Black bat
                    else:
                        r, g, b = 245, 195, 30 # Golden Bat-signal background
                        
            elif symbol_type == "spiderman":
                # Spider-Man web pattern & spider emblem
                angle = math.atan2(rel_y, rel_x)
                web_spoke = abs(math.sin(angle * 8.0)) < 0.1 and dist < 140
                web_ring = abs(math.sin(dist * 0.15)) < 0.15 and dist < 140
                if web_spoke or web_ring:
                    r, g, b = 30, 30, 35 # Web lines
                elif dist < 45:
                    # Central Spider emblem
                    r, g, b = 20, 20, 25
                elif dist < 90 and (abs(rel_x - 30) < 25 or abs(rel_x + 30) < 25) and abs(rel_y) < 30:
                    # Large white expressive mask eyes
                    r, g, b = 250, 250, 255
                    
            elif symbol_type == "goku":
                # Goku Super Saiyan golden aura & 4-Star Dragon Ball
                ball_dist = math.sqrt(rel_x*rel_x + rel_y*rel_y)
                if ball_dist < 85:
                    # Orange crystal sphere
                    r, g, b = 245, 130, 20
                    # 4 Red stars inside
                    for sx, sy in [(-25, 0), (25, 0), (0, -25), (0, 25)]:
                        s_dist = math.sqrt((rel_x - sx)**2 + (rel_y - sy)**2)
                        if s_dist < 12:
                            r, g, b = 210, 25, 25
                else:
                    # Golden Super Saiyan radiating energy rays
                    angle = math.atan2(rel_y, rel_x)
                    rays = math.cos(angle * 12.0) * 0.5 + 0.5
                    r = clamp(r + rays * 120)
                    g = clamp(g + rays * 100)
                    b = clamp(b + rays * 10)

            # Left power gem (e.g. 9 Attack) and Right power gem (e.g. 8 Defense)
            gem_l = math.sqrt((x - 80)**2 + (y - 80)**2) < 24
            gem_r = math.sqrt((x - (w - 80))**2 + (y - 80)**2) < 24
            if gem_l:
                r, g, b = 230, 45, 45 # Red attack crystal
            elif gem_r:
                r, g, b = 45, 120, 240 # Blue defense crystal
                
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    filename = f"GLProject/assets/textures/card_{symbol_type}.png"
    write_png(filename, w, h, buf)

def create_poster():
    """Generates Vincent van Gogh's 'The Starry Night' artwork texture."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    
    for y in range(h):
        for x in range(w):
            nx = x / float(w)
            ny = y / float(h)
            
            # Swirling night sky background
            swirl1 = math.sin(nx * 8.0 + math.sin(ny * 5.0) * 2.0)
            swirl2 = math.cos(ny * 6.0 + math.cos(nx * 4.0) * 1.5)
            
            # Palette: Deep indigo, cobalt blue, gold yellow, olive cypress
            r = 25 + int(35 * swirl1 + 25 * swirl2)
            g = 45 + int(45 * swirl1 + 35 * swirl2)
            b = 110 + int(70 * swirl1 + 45 * swirl2)
            
            # Glowing Crescent Moon in top-right
            moon_dx = x - (w - 90)
            moon_dy = y - (h - 90)
            moon_dist = math.sqrt(moon_dx*moon_dx + moon_dy*moon_dy)
            if moon_dist < 55:
                halo = max(0.0, 1.0 - moon_dist / 55.0)
                r = clamp(r + halo * 220)
                g = clamp(g + halo * 200)
                b = clamp(b + halo * 70)
                # Crescent moon cut
                if moon_dist < 32 and (moon_dx + 12)**2 + moon_dy**2 > 26**2:
                    r, g, b = 255, 245, 140
                    
            # Swirling Stars
            for sx, sy in [(120, 380), (220, 420), (330, 340), (160, 280), (420, 240)]:
                s_dist = math.sqrt((x - sx)**2 + (y - sy)**2)
                if s_dist < 40:
                    s_halo = (math.cos(s_dist * 0.18) * 0.5 + 0.5) * max(0.0, 1.0 - s_dist / 40.0)
                    r = clamp(r + s_halo * 210)
                    g = clamp(g + s_halo * 190)
                    b = clamp(b + s_halo * 50)
                    if s_dist < 8:
                        r, g, b = 255, 255, 220
                        
            # Dark Cypress tree silhouette in foreground (left)
            cypress_width = (y / float(h)) * 60 + 25
            if x < cypress_width + math.sin(y * 0.1) * 12:
                # Dark olive-green/black cypress with flame curves
                r = 18 + int(8 * math.sin(y * 0.2))
                g = 32 + int(12 * math.cos(x * 0.3))
                b = 22
                
            # Village hills at bottom
            if y < 110:
                hill = math.sin(x * 0.02) * 25 + 75
                if y < hill:
                    r, g, b = 15, 22, 45 # Dark village silhouette
                    # Small warm yellow church & cottage windows
                    if (x % 50 < 8) and (y > 25 and y < 45):
                        r, g, b = 245, 210, 80
            
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/poster_starry_night.png", w, h, buf)

def create_bookshelf_texture():
    """Generates wood shelf texture with rows of colorful books for Decor 2."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    
    num_shelves = 4
    shelf_h = h // num_shelves
    
    for y in range(h):
        shelf_idx = y // shelf_h
        y_in_shelf = y % shelf_h
        for x in range(w):
            # Shelf wood planks
            if y_in_shelf < 14 or y_in_shelf > shelf_h - 6:
                # Oak wooden horizontal plank
                r, g, b = 95, 55, 30
            elif x < 16 or x > w - 17:
                # Vertical side wood frame
                r, g, b = 85, 48, 25
            else:
                # Books lined up!
                book_idx = (x - 16) // 22
                x_in_book = (x - 16) % 22
                book_height = 80 + (book_idx * 37) % 35
                
                if y_in_shelf < book_height:
                    # Book colors: Crimson, Emerald, Sapphire, Gold, Leather Brown
                    colors = [
                        (160, 35, 40),
                        (30, 110, 60),
                        (35, 65, 145),
                        (170, 130, 40),
                        (90, 45, 25),
                        (120, 40, 110)
                    ]
                    base_c = colors[book_idx % len(colors)]
                    # Book spine curve shading
                    spine_shade = math.sin((x_in_book / 22.0) * math.pi)
                    r = clamp(base_c[0] * (0.6 + 0.5 * spine_shade))
                    g = clamp(base_c[1] * (0.6 + 0.5 * spine_shade))
                    b = clamp(base_c[2] * (0.6 + 0.5 * spine_shade))
                    # Gold embossed band on spine
                    if y_in_shelf in (25, 26, 65, 66):
                        r, g, b = 215, 175, 45
                else:
                    # Dark shadow in back of shelf
                    r, g, b = 25, 20, 18
                    
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
            
    write_png("GLProject/assets/textures/bookshelf.png", w, h, buf)

def create_shield_texture():
    """Generates Medieval Knight Shield texture for Decor 3."""
    w, h = 512, 512
    buf = bytearray(w * h * 4)
    cx, cy = w // 2, h // 2
    
    for y in range(h):
        for x in range(w):
            dx = (x - cx) / 200.0
            dy = (y - cy) / 220.0
            
            # Shield shape formula (pointed bottom)
            in_shield = False
            if dy > -0.8 and dy < 0.2 and abs(dx) < 0.85:
                in_shield = True
            elif dy >= 0.2 and dy < 0.95:
                # Tapering to pointed tip
                taper = (0.95 - dy) / 0.75
                if abs(dx) < 0.85 * math.sqrt(max(0.0, taper)):
                    in_shield = True
                    
            if not in_shield:
                # Transparent outside shield
                r, g, b, a = 0, 0, 0, 0
            else:
                # Steel rim with golden rivets
                border_dist = min(abs(dx), abs(dy))
                is_rim = abs(dx) > 0.72 or dy < -0.68 or (dy > 0.15 and abs(dx) > 0.65 * math.sqrt(max(0.0, (0.95-dy)/0.75)))
                
                if is_rim:
                    r, g, b, a = 190, 195, 205, 255 # Polished steel rim
                else:
                    # Quartered Heraldic Crest: Top-left Royal Red, Top-right Royal Gold, Bottom-left Gold, Bottom-right Red
                    quarter_red = (dx < 0 and dy < 0.1) or (dx >= 0 and dy >= 0.1)
                    if quarter_red:
                        r, g, b, a = 175, 25, 30, 255 # Crimson
                    else:
                        r, g, b, a = 220, 180, 45, 255 # Heraldic gold
                        
                    # Central Silver Lion / Cross Emblem
                    dist_c = math.sqrt(dx*dx + (dy - 0.05)**2)
                    if dist_c < 0.35:
                        cross = abs(dx) < 0.08 or abs(dy - 0.05) < 0.08
                        if cross:
                            r, g, b, a = 240, 245, 250, 255 # Silver cross
                            
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = a
            
    write_png("GLProject/assets/textures/shield.png", w, h, buf)

def create_gold_chalice_texture():
    """Generates an ornate gilded metal texture for the 3D model (Decor 1)."""
    w, h = 256, 256
    buf = bytearray(w * h * 4)
    for y in range(h):
        for x in range(w):
            spec = math.sin(x * 0.2) * math.cos(y * 0.1) * 25
            r = 215 + spec
            g = 175 + spec * 0.8
            b = 55 + spec * 0.3
            idx = (y * w + x) * 4
            buf[idx] = clamp(r)
            buf[idx+1] = clamp(g)
            buf[idx+2] = clamp(b)
            buf[idx+3] = 255
    write_png("GLProject/assets/textures/chalice_gold.png", w, h, buf)

def create_chalice_3d_model():
    """Generates a clean Wavefront .OBJ 3D model of an ornate antique chalice/goblet."""
    # We create a lathe of revolution for the goblet:
    # Profile points: (radius, y)
    profile = [
        (0.0, 0.0),      # base bottom center
        (0.45, 0.0),     # base foot edge
        (0.42, 0.04),    # base bevel
        (0.35, 0.08),    # base slope
        (0.12, 0.18),    # stem bottom
        (0.10, 0.35),    # stem lower
        (0.18, 0.45),    # ornamental knop center
        (0.10, 0.55),    # stem upper
        (0.14, 0.62),    # cup base
        (0.38, 0.78),    # cup swell
        (0.42, 0.98),    # cup rim outer
        (0.39, 0.98),    # cup rim inner
        (0.35, 0.80),    # cup inner wall
        (0.12, 0.65),    # cup inner bottom
        (0.0, 0.65)      # cup inner center
    ]
    
    segments = 24
    vertices = []
    normals = []
    uvs = []
    faces = []
    
    # Generate vertices
    for p_idx, (r, y) in enumerate(profile):
        v_coord = y
        for s in range(segments):
            angle = (2.0 * math.pi * s) / segments
            x = r * math.cos(angle)
            z = r * math.sin(angle)
            u_coord = s / float(segments)
            
            # Normal calculation approximation
            if r > 0.001:
                nx = math.cos(angle)
                nz = math.sin(angle)
                ny = 0.2
            else:
                nx, ny, nz = 0.0, 1.0, 0.0
            mag = math.sqrt(nx*nx + ny*ny + nz*nz)
            nx, ny, nz = nx/mag, ny/mag, nz/mag
            
            vertices.append((x, y, z))
            normals.append((nx, ny, nz))
            uvs.append((u_coord, v_coord))
            
    # Generate quad faces between adjacent profile rings
    for p in range(len(profile) - 1):
        for s in range(segments):
            next_s = (s + 1) % segments
            
            i1 = p * segments + s + 1
            i2 = p * segments + next_s + 1
            i3 = (p + 1) * segments + next_s + 1
            i4 = (p + 1) * segments + s + 1
            
            faces.append((i1, i2, i3, i4))
            
    # Write OBJ file
    obj_path = "GLProject/assets/models/chalice.obj"
    with open(obj_path, "w") as f:
        f.write("# Wavefront OBJ: Antique Ornate Chalice / Goblet Decor Model\n")
        f.write("mtllib chalice.mtl\n")
        f.write("o Chalice\n")
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
            
    # Write MTL file
    mtl_path = "GLProject/assets/models/chalice.mtl"
    with open(mtl_path, "w") as f:
        f.write("newmtl GoldMaterial\n")
        f.write("Ka 0.25 0.20 0.07\n")
        f.write("Kd 0.85 0.70 0.22\n")
        f.write("Ks 0.90 0.85 0.60\n")
        f.write("Ns 128.0\n")
        f.write("map_Kd ../textures/chalice_gold.png\n")
        
    print(f"Generated 3D Model: {obj_path} and {mtl_path}")

def main():
    print("--- Generating All High-Quality Assets for Computer Graphics Project ---")
    create_wood_floor()
    create_wallpaper()
    create_table_texture()
    create_card_back()
    
    # 5 Favorite Fictional Characters:
    # 1. Geralt of Rivia (The Witcher)
    create_card_front("Geralt of Rivia", (60, 65, 80), (220, 220, 240), "geralt")
    # 2. Gandalf the Grey (Lord of the Rings)
    create_card_front("Gandalf", (50, 60, 95), (200, 220, 255), "gandalf")
    # 3. Batman (DC Comics)
    create_card_front("Batman", (20, 25, 35), (240, 190, 30), "batman")
    # 4. Spider-Man (Marvel)
    create_card_front("Spider-Man", (180, 25, 35), (30, 80, 180), "spiderman")
    # 5. Son Goku (Dragon Ball)
    create_card_front("Son Goku", (230, 110, 15), (250, 220, 30), "goku")
    
    # Room Decor Assets:
    create_poster()
    create_bookshelf_texture()
    create_shield_texture()
    create_gold_chalice_texture()
    create_chalice_3d_model()
    print("--- All Assets Generated Successfully! ---")

if __name__ == "__main__":
    main()
