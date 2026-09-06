#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
from pathlib import Path
from typing import List, Dict

import matplotlib.pyplot as plt

def find_latest_csv(logdir: Path) -> Path:
    csv_files = sorted(logdir.glob("*.CSV"), key = lambda p : p.stat().st_mtime)
    if not csv_files:
        raise FileNotFoundError(f"No CSV files found in {logdir}")
    return csv_files[-1]

def read_log(path: Path) -> List[Dict[str, str]]:
    rows: List[Dict[str, str]] = []
    with path.open("r", newline = "", encoding = "utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)
    return rows

def to_int(row: Dict[str, str], key: str) -> int:
    return int(float(row[key]))

def main() -> None:
    parser = argparse.ArgumentParser(description = "Plot NeutronCascade population/fission from CSV log.")
    parser.add_argument("--file", type=str, default="", help="Path to CSV file. If empty, use latest in logs/.")
    parser.add_argument("--logs", type = str, default = "logs", help = "Log directory");
    args = parser.parse_args()

    log_dir = Path(args.logs)
    csv_path = Path(args.file) if args.file else find_latest_csv(log_dir)

    rows = read_log(csv_path)
    if not rows:
        raise RuntimeError(f"No data found in {csv_path}")

    ticks = [to_int(r, "Tick") for r in rows]
    populations = [to_int(r, "Population") for r in rows]
    fissions = [to_int(r, "Fissions") for r in rows]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize =(10, 7), sharex=True)
    fig.suptitle(f"Neutron Cascade Log: ({csv_path.name})")

    ax1.plot(ticks, populations, linewidth = 2)
    ax1.set_ylabel("Population")
    ax1.grid(True, alpha = 0.3)

    ax2.plot(ticks, fissions, linewidth = 2, color = "orange")
    ax2.set_ylabel("Fissions")
    ax2.set_xlabel("Tick")
    ax2.grid(True, alpha = 0.3)

    plt.tight_layout()
    
    out_path = Path("docs/population_plot.png")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(out_path, dpi=150, bbox_inches="tight")
    print(f"Saved plot image to: {out_path}")

    plt.show()

if __name__ == "__main__":
    main()