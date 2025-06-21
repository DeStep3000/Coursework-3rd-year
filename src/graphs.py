import os

import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results.csv")
df.rename(columns={"Time": "Time(us)"}, inplace=True)
algorithms = df["Algorithm"].unique()
os.makedirs("../graphs", exist_ok=True)

for algo in algorithms:
    data = df[df["Algorithm"] == algo]

    plt.figure(figsize=(8, 4))
    plt.bar(data["Dataset"], data["Time(us)"])
    plt.title(f"{algo} — Time")
    plt.xlabel("Dataset")
    plt.ylabel("Time (μs)")
    if "DP" in algo:
        plt.yscale("log")
    plt.tight_layout()
    plt.grid(axis="y", linestyle="--", alpha=0.4)
    plt.savefig(f"../graphs/time_{algo.replace(' ', '_')}.png")
    plt.close()

    plt.figure(figsize=(8, 4))
    plt.bar(data["Dataset"], data["Match (%)"])
    plt.title(f"{algo} — Match (%)")
    plt.xlabel("Dataset")
    plt.ylabel("Match (%)")
    plt.tight_layout()
    plt.grid(axis="y", linestyle="--", alpha=0.4)
    plt.savefig(f"../graphs/match_{algo.replace(' ', '_')}.png")
    plt.close()

avg_df = df.groupby("Algorithm").agg(
    {"Time(us)": "mean", "Match (%)": "mean"}).reset_index()

plt.figure(figsize=(8, 4))
plt.bar(avg_df["Algorithm"], avg_df["Time(us)"])
plt.title("Average Time per Algorithm")
plt.xlabel("Algorithm")
plt.ylabel("Time (μs)")
plt.yscale("log")
plt.tight_layout()
plt.grid(axis="y", linestyle="--", alpha=0.4)
plt.savefig("../graphs/avg_time.png")
plt.close()

plt.figure(figsize=(8, 4))
plt.bar(avg_df["Algorithm"], avg_df["Match (%)"])
plt.title("Average Match (%) per Algorithm")
plt.xlabel("Algorithm")
plt.ylabel("Match (%)")
plt.ylim(0, 110)
plt.tight_layout()
plt.grid(axis="y", linestyle="--", alpha=0.4)
plt.savefig("../graphs/avg_match.png")
plt.close()
