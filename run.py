import argparse
import importlib
import json
import os

from dotenv import load_dotenv

import audio
from transform import transform


def resolve_trans_function(name):
    try:
        module = importlib.import_module(f"transforms.{name}")
        fn = getattr(module, name)
    except (ImportError, AttributeError) as e:
        raise SystemExit(f"unknown trans function: {name}") from e
    return fn


def _from_env(name):
    raw = os.environ.get(name)
    if raw is None:
        raise SystemExit(f"environment variable {name} is not set (params.env)")
    return raw


def main():
    load_dotenv("params.env")
    p = argparse.ArgumentParser()
    p.add_argument("input")
    p.add_argument("output")
    p.add_argument("--trans", required=True)
    p.add_argument("--params", default="{}", help="JSON dict for the trans function")
    p.add_argument("--width", required=True, type=int)
    p.add_argument("--height", required=True, type=int)
    p.add_argument("--fps", required=True, type=int)
    p.add_argument("--codec", default=None, help="override CODEC from params.env")
    p.add_argument("--quality", default=None, type=int, help="override QUALITY from params.env")
    p.add_argument("--verbose", action="store_true", help="print stage progress")
    args = p.parse_args()

    codec = args.codec if args.codec is not None else _from_env("CODEC")
    codec_src = "args" if args.codec is not None else "params.env"
    quality = args.quality if args.quality is not None else int(_from_env("QUALITY"))
    quality_src = "args" if args.quality is not None else "params.env"

    if args.verbose:
        print(
            f"config: codec={codec} ({codec_src}), quality={quality} ({quality_src}), "
            f"caps={os.environ.get('MAX_WIDTH')}x{os.environ.get('MAX_HEIGHT')}"
            f"@{os.environ.get('MAX_FPS')}"
        )

    trans_function = resolve_trans_function(args.trans)
    params = json.loads(args.params)
    if args.verbose:
        print(f"loading audio clip: {args.input}")
    clip = audio.load(args.input)
    if args.verbose:
        samples, sr = clip
        channels = samples.shape[0] if samples.ndim == 2 else 1
        print(
            f"loaded: {audio.duration(samples, sr):.2f}s, {sr} Hz, {channels} ch"
        )
    out = transform(
        clip, trans_function, params, args.width, args.height, args.output, args.fps,
        codec, quality,
        args.input,
        args.verbose,
    )
    if args.verbose:
        size_mb = os.path.getsize(out) / (1024 * 1024)
        print(f"complete: {out} ({size_mb:.1f} MB)")


if __name__ == "__main__":
    main()
