#!/usr/bin/env -S uv run --script
# /// script
# requires-python = ">=3.9"
# dependencies = [
#     "matplotlib>=3.5",
#     "numpy>=1.21",
#     "pandas>=1.3",
# ]
# ///
"""Genera los gráficos de la Tarea 2 a partir de los CSV de outputs/results/.

Lee los archivos producidos por el ejecutable `tarea2` y escribe figuras PNG
en outputs/plots/. Cada gráfico incluye, cuando corresponde, las cotas teóricas
multiplicadas por una constante ajustada por mínimos cuadrados (recomendación
(b) del enunciado), para comparar visualmente con los resultados empíricos.

Uso:
    uv run scripts/plot.py

Si no se tiene uv:
    pip install -r requirements.txt && python3 scripts/plot.py

"""

import os
import math

import numpy as np
import pandas as pd
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS = "outputs/results"
PLOTS = "outputs/plots"


def path(name):
    return os.path.join(RESULTS, name)


def exists(name):
    return os.path.isfile(path(name))


def save(fig, name):
    os.makedirs(PLOTS, exist_ok=True)
    out = os.path.join(PLOTS, name)
    fig.tight_layout()
    fig.savefig(out, dpi=130)
    plt.close(fig)
    print(f"  -> {out}")


def fit_constant(y, shape):
    """Constante c que mejor ajusta c*shape a y (mínimos cuadrados)."""
    shape = np.asarray(shape, dtype=float)
    y = np.asarray(y, dtype=float)
    denom = np.dot(shape, shape)
    if denom == 0:
        return 0.0
    return float(np.dot(shape, y) / denom)


# 7.2  Escenarios base

def plot_base():
    if not exists("base_totals.csv"):
        return
    df = pd.read_csv(path("base_totals.csv"))
    configs = ["a", "b", "c", "d"]
    titles = {
        "a": "(a) Inserción aleatoria, búsqueda uniforme",
        "b": "(b) Inserción aleatoria, búsqueda sesgada",
        "c": "(c) Inserción ordenada, búsqueda uniforme",
        "d": "(d) Inserción ordenada, búsqueda sesgada",
    }

    # --- Costo medio por búsqueda vs N ---
    fig, axes = plt.subplots(2, 2, figsize=(13, 9))
    for ax, cfg in zip(axes.flat, configs):
        sub = df[df["config"] == cfg]
        for struct, marker in [("AVL", "o"), ("Splay", "s")]:
            s = sub[sub["struct"] == struct].sort_values("N")
            ax.plot(s["N"], s["avg_search_cost"], marker=marker, label=struct)
        ns = np.sort(sub["N"].unique())
        ref = fit_constant(
            sub[sub["struct"] == "AVL"].sort_values("N")["avg_search_cost"],
            np.log2(ns),
        )
        ax.plot(ns, ref * np.log2(ns), "k--", alpha=0.5, label="c·log₂N")
        ax.set_title(titles[cfg])
        ax.set_xlabel("N")
        ax.set_ylabel("Costo medio (nodos visitados / búsqueda)")
        ax.set_xscale("log", base=2)
        ax.legend()
        ax.grid(True, alpha=0.3)
    fig.suptitle("7.2 — Costo medio por búsqueda (AVL vs Splay)")
    save(fig, "base_avg_cost.png")

    # --- Tiempo medio por búsqueda vs N ---
    fig, axes = plt.subplots(2, 2, figsize=(13, 9))
    for ax, cfg in zip(axes.flat, configs):
        sub = df[df["config"] == cfg]
        for struct, marker in [("AVL", "o"), ("Splay", "s")]:
            s = sub[sub["struct"] == struct].sort_values("N")
            # tiempo total de búsqueda / M ; M = 10^c * N con c = 1 -> 10N
            m = 10 * s["N"]
            ax.plot(s["N"], s["search_time_ns"] / m, marker=marker, label=struct)
        ax.set_title(titles[cfg])
        ax.set_xlabel("N")
        ax.set_ylabel("Tiempo medio por búsqueda (ns)")
        ax.set_xscale("log", base=2)
        ax.legend()
        ax.grid(True, alpha=0.3)
    fig.suptitle("7.2 — Tiempo medio por búsqueda (AVL vs Splay)")
    save(fig, "base_avg_time.png")

    # --- Naturaleza amortizada: promedio acumulado por búsqueda ---
    fig, axes = plt.subplots(2, 2, figsize=(13, 9))
    for ax, cfg in zip(axes.flat, configs):
        f = f"base_persearch_{cfg}.csv"
        if not exists(f):
            continue
        s = pd.read_csv(path(f))
        ax.plot(s["idx"], s["avl_cumavg"], label="AVL (prom. acumulado)")
        ax.plot(s["idx"], s["splay_cumavg"], label="Splay (prom. acumulado)")
        ax.set_title(titles[cfg])
        ax.set_xlabel("Índice de búsqueda")
        ax.set_ylabel("Costo promedio acumulado")
        ax.legend()
        ax.grid(True, alpha=0.3)
    fig.suptitle("7.2 — Naturaleza amortizada del Splay Tree (N máx)")
    save(fig, "base_amortized.png")


# 7.3.1.a  Sequential Access Theorem

def plot_seq():
    if not exists("seq_access.csv"):
        return
    df = pd.read_csv(path("seq_access.csv")).sort_values("m")
    m = df["m"].to_numpy(dtype=float)
    n = float(df["n"].iloc[0])

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5.5))

    # Costo
    ax1.plot(m, df["avl_cost"], "o-", label="AVL (empírico)")
    ax1.plot(m, df["splay_cost"], "s-", label="Splay (empírico)")
    shape_avl = m * math.log2(n)
    shape_splay = m * np.log2(n / m)
    c_avl = fit_constant(df["avl_cost"], shape_avl)
    c_splay = fit_constant(df["splay_cost"], shape_splay)
    ax1.plot(m, c_avl * shape_avl, "k--", alpha=0.6, label="c·m·log₂n")
    ax1.plot(m, c_splay * shape_splay, "r--", alpha=0.6, label="c·m·log₂(n/m)")
    ax1.set_title("Sequential Access — Costo total")
    ax1.set_xlabel("m (número de accesos crecientes)")
    ax1.set_ylabel("Costo total (nodos visitados)")
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    # Tiempo
    ax2.plot(m, df["avl_time_ns"] / 1e6, "o-", label="AVL")
    ax2.plot(m, df["splay_time_ns"] / 1e6, "s-", label="Splay")
    ax2.set_title("Sequential Access — Tiempo total")
    ax2.set_xlabel("m (número de accesos crecientes)")
    ax2.set_ylabel("Tiempo total (ms)")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    fig.suptitle(f"7.3.1.a — Sequential Access Theorem (n = {int(n)})")
    save(fig, "seq_access.png")


# 7.3.1.b  Working Set Theorem

def plot_ws():
    if not exists("working_set.csv"):
        return
    df = pd.read_csv(path("working_set.csv")).sort_values("W")
    W = df["W"].to_numpy(dtype=float)
    n = float(df["n"].iloc[0])

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5.5))

    ax1.plot(W, df["avl_avg_cost"], "o-", label="AVL (empírico)")
    ax1.plot(W, df["splay_avg_cost"], "s-", label="Splay (empírico)")
    shape = np.log2(W)
    c = fit_constant(df["splay_avg_cost"], shape)
    ax1.plot(W, c * shape, "r--", alpha=0.6, label="c·log₂W")
    ax1.axhline(math.log2(n), color="k", ls="--", alpha=0.5, label="log₂n")
    ax1.set_xscale("log")
    ax1.set_title("Working Set — Costo medio por búsqueda")
    ax1.set_xlabel("W (tamaño del working set)")
    ax1.set_ylabel("Costo medio (nodos visitados)")
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    ax2.plot(W, df["avl_time_ns"] / 1e6, "o-", label="AVL")
    ax2.plot(W, df["splay_time_ns"] / 1e6, "s-", label="Splay")
    ax2.set_xscale("log")
    ax2.set_title("Working Set — Tiempo total")
    ax2.set_xlabel("W (tamaño del working set)")
    ax2.set_ylabel("Tiempo total (ms)")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    fig.suptitle(f"7.3.1.b — Working Set Theorem (n = {int(n)})")
    save(fig, "working_set.png")


# 7.4  Traversal Conjecture (bonus)

def plot_traversal():
    if not exists("traversal.csv"):
        return
    df = pd.read_csv(path("traversal.csv")).sort_values("idx")

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5.5))

    ax1.plot(df["idx"], df["cost"], ".", ms=3, alpha=0.5, label="Costo por búsqueda")
    # media móvil
    window = max(1, len(df) // 50)
    roll = df["cost"].rolling(window, min_periods=1).mean()
    ax1.plot(df["idx"], roll, "r-", label=f"Media móvil ({window})")
    ax1.set_title("Traversal Conjecture — Costo por búsqueda en T₂")
    ax1.set_xlabel("Índice en la secuencia (preorden de T₁)")
    ax1.set_ylabel("Costo (nodos visitados)")
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    ax2.plot(df["idx"], df["cum_time_ns"] / 1e6, "-")
    ax2.set_title("Traversal Conjecture — Tiempo acumulado")
    ax2.set_xlabel("Índice en la secuencia")
    ax2.set_ylabel("Tiempo acumulado (ms)")
    ax2.grid(True, alpha=0.3)

    fig.suptitle("7.4 — Traversal Conjecture")
    save(fig, "traversal.png")


def main():
    print("Generando gráficos en outputs/plots/ ...")
    plot_base()
    plot_seq()
    plot_ws()
    plot_traversal()
    print("Listo.")


if __name__ == "__main__":
    main()
