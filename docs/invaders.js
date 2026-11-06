// A few invaders loitering in the bottom left corner of the page.
// Each sprite has two frames, swapped in time like the formation in the game.

const SPRITES = [
    [
        [
            "...##...",
            "..####..",
            ".######.",
            "##.##.##",
            "########",
            "..#..#..",
            ".#.##.#.",
            "#.#..#.#",
        ],
        [
            "...##...",
            "..####..",
            ".######.",
            "##.##.##",
            "########",
            ".#.##.#.",
            "#......#",
            ".#....#.",
        ],
    ],
    [
        [
            "..#.....#..",
            "...#...#...",
            "..#######..",
            ".##.###.##.",
            "###########",
            "#.#######.#",
            "#.#.....#.#",
            "...##.##...",
        ],
        [
            "..#.....#..",
            "#..#...#..#",
            "#.#######.#",
            "###.###.###",
            "###########",
            ".#########.",
            "..#.....#..",
            ".#.......#.",
        ],
    ],
    [
        [
            "....####....",
            ".##########.",
            "############",
            "###..##..###",
            "############",
            "...##..##...",
            "..##.##.##..",
            "##........##",
        ],
        [
            "....####....",
            ".##########.",
            "############",
            "###..##..###",
            "############",
            "..###..###..",
            ".##..##..##.",
            "..##....##..",
        ],
    ],
];

const FRAME_MS = 400;   // how often the sprites swap frames
const STEP_MS  = 90;    // length of one jerky movement step

const SVG_NS = "http://www.w3.org/2000/svg";

function drawFrame(rows) {
    const group = document.createElementNS(SVG_NS, "g");
    rows.forEach((row, y) => {
        [...row].forEach((cell, x) => {
            if (cell !== "#") return;
            const pixel = document.createElementNS(SVG_NS, "rect");
            pixel.setAttribute("x", x);
            pixel.setAttribute("y", y);
            pixel.setAttribute("width", 1);
            pixel.setAttribute("height", 1);
            group.append(pixel);
        });
    });
    return group;
}

function drawInvader(frames) {
    const width = frames[0][0].length;
    const svg = document.createElementNS(SVG_NS, "svg");
    svg.setAttribute("viewBox", `0 0 ${width} 8`);
    svg.setAttribute("shape-rendering", "crispEdges");
    svg.style.setProperty("--pixels", width);
    frames.forEach((rows) => svg.append(drawFrame(rows)));
    return svg;
}

const troupe = document.querySelector(".invaders");
const text = document.querySelector(".text");
const invaders = SPRITES.map(drawInvader);

// Which way each edge of the window turns an invader, so its feet face the edge
const EDGES = { bottom: 0, top: 180, left: 90, right: -90 };
const CORNER = 16;   // keep this far from the corners of the window

const pause = (ms) => new Promise((resolve) => setTimeout(resolve, ms));
const pick = (list) => list[Math.floor(Math.random() * list.length)];

// Takes the stretch from..to out of each range, keeping whatever is left either side
function cut(ranges, from, to) {
    return ranges.flatMap(([start, end]) => [[start, Math.min(end, from)], [Math.max(start, to), end]]);
}

// Where along an edge a spot of the given length can be centred, as [start, end] ranges
function room(edge, length) {
    const { clientWidth, clientHeight } = document.documentElement;
    const size = edge === "left" || edge === "right" ? clientHeight : clientWidth;
    let ranges = [[CORNER + length / 2, size - CORNER - length / 2]];

    // Never along the bottom of the text being read
    if (edge === "bottom") {
        const { left, right } = text.getBoundingClientRect();
        ranges = cut(ranges, left - length / 2 - CORNER, right + length / 2 + CORNER);
    }
    return ranges.filter(([start, end]) => end >= start);
}

// Picks an edge at random, then a point along it with room for the spot
function findSpot(length) {
    const edges = Object.keys(EDGES).filter((edge) => room(edge, length).length);
    if (!edges.length) return null;

    const edge = pick(edges);
    const ranges = room(edge, length);
    let distance = Math.random() * ranges.reduce((sum, [start, end]) => sum + end - start, 0);
    for (const [start, end] of ranges) {
        if (distance <= end - start) return { edge, centre: start + distance };
        distance -= end - start;
    }
    return { edge, centre: ranges[0][0] };
}

// Stands the spot on the edge, turned so the bottom of the spot is the edge
function place(spot, { edge, centre }) {
    const { clientWidth, clientHeight } = document.documentElement;
    const width = spot.offsetWidth;
    const height = spot.offsetHeight;
    const [x, y] = {
        bottom: [centre, clientHeight - height / 2],
        top:    [centre, height / 2],
        left:   [height / 2, centre],
        right:  [clientWidth - height / 2, centre],
    }[edge];
    spot.style.transform = `translate(${x}px, ${y}px) rotate(${EDGES[edge]}deg) translate(${-width / 2}px, ${-height / 2}px)`;
}

// Moves an invader in jerky steps, resolving when it arrives.
// Its own y axis points into the edge it stands on.
async function move(invader, x, y, ms) {
    const transform = `translate(${x}px, ${y}px)`;
    const steps = Math.max(1, Math.round(ms / STEP_MS));
    const animation = invader.animate({ transform }, { duration: ms, easing: `steps(${steps})`, fill: "forwards" });
    await animation.finished;
    invader.style.transform = transform;
    animation.cancel();
}

// Puts a group of invaders, hidden behind the edge, somewhere with `extra`
// room to move sideways, lets them perform, then clears them away
async function appear(group, extra, act) {
    const spot = document.createElement("div");
    spot.className = "spot";
    spot.append(...group);
    troupe.append(spot);

    const depth = spot.offsetHeight;
    const where = findSpot(spot.offsetWidth + extra);
    if (where) {
        group.forEach((invader) => {
            invader.style.transform = `translate(0, ${depth}px)`;
        });
        place(spot, where);
        await act(group, depth, extra);
    }
    spot.remove();
}

// Pokes its head out, has a look around, then ducks back
async function peek([invader], depth) {
    const head = depth * 0.5;
    await move(invader, 0, head, 500);
    await pause(500);
    await move(invader, -8, head, 180);
    await pause(350);
    await move(invader, 8, head, 180);
    await pause(350);
    await move(invader, 0, head, 180);
    await move(invader, 0, depth, 180);
}

// Jumps out of the edge and falls straight back in
async function hop([invader], depth) {
    await move(invader, 0, -depth * 1.5, 450);
    await pause(150);
    await move(invader, 0, depth, 270);
}

// Climbs out, wanders along the edge, thinks better of it, and dives back in
async function stroll([invader], depth, distance) {
    invader.style.transform = `translate(${-distance / 2}px, ${depth}px)`;
    await move(invader, -distance / 2, 0, 270);
    await move(invader, distance / 2, 0, 1800);
    await pause(900);
    await move(invader, distance / 2, depth, 270);
}

// Everyone climbs out and sits for a while, then leaves one by one
async function sit(group, depth) {
    for (const invader of group) {
        await move(invader, 0, 0, 500);
    }
    await pause(3500);
    for (const invader of [...group].reverse()) {
        await move(invader, 0, depth, 270);
    }
}

const ACTS = [
    () => appear([pick(invaders)], 16, peek),
    () => appear([pick(invaders)], 16, peek),
    () => appear([pick(invaders)], 0, hop),
    () => appear([pick(invaders)], 160, stroll),
    () => appear(invaders, 0, sit),
];

async function perform() {
    await pause(800);
    await appear(invaders, 0, sit);

    while (true) {
        await pause(1500 + Math.random() * 3500);
        await pick(ACTS)();
    }
}

// If motion is unwelcome the invaders just sit still, somewhere along an edge
if (matchMedia("(prefers-reduced-motion: reduce)").matches) {
    const spot = document.createElement("div");
    spot.className = "spot";
    spot.append(...invaders);
    troupe.append(spot);
    const where = findSpot(spot.offsetWidth);
    if (where) place(spot, where);
    else spot.remove();
} else {
    setInterval(() => troupe.classList.toggle("alternate"), FRAME_MS);
    perform();
}
