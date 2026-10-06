import os

import imageio_ffmpeg

from render import Canvas


def _cap(name):
    raw = os.environ.get(name)
    if raw is None:
        raise RuntimeError(f"environment variable {name} is not set")
    return int(raw)


def transform(
    audio_clip, trans_function, params, width, height, out, fps, codec, quality,
    audio_path,
    verbose,
):
    max_width = _cap("MAX_WIDTH")
    max_height = _cap("MAX_HEIGHT")
    max_fps = _cap("MAX_FPS")
    if not 0 < width <= max_width:
        raise ValueError(f"width must be in 1..{max_width}, got {width}")
    if not 0 < height <= max_height:
        raise ValueError(f"height must be in 1..{max_height}, got {height}")
    if not 0 < fps <= max_fps:
        raise ValueError(f"fps must be in 1..{max_fps}, got {fps}")

    samples, sr = audio_clip
    canvas = Canvas(width, height)
    dt = 1 / fps
    n_frames = int(round(samples.shape[-1] / sr * fps))
    audio_codec = "copy" if audio_path.lower().endswith((".m4a", ".aac")) else "aac"
    if verbose:
        print(
            f"creating writer: {out} ({width}x{height} @{fps} fps, "
            f"codec={codec}, quality={quality}, audio={audio_codec})"
        )
    writer = imageio_ffmpeg.write_frames(
        out, (width, height), fps=fps, codec=codec, quality=quality,
        pix_fmt_in="rgb24",
        audio_path=audio_path, audio_codec=audio_codec,
        output_params=["-shortest"],
    )
    writer.send(None)
    for i in range(n_frames):
        canvas.clear((0, 0, 0))
        trans_function(audio_clip, i * dt, dt, canvas, params)
        writer.send(canvas.frame().tobytes())
        if verbose and ((i + 1) == 1 or (i + 1) % 100 == 0):
            print(f"frame {i + 1}/{n_frames} ok")
    if verbose:
        print(f"finalizing: {out}")
    writer.close()
    return out
