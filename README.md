# Automated Power and Rate Control in 802.11 Wireless Networks

## Project Overview
In modern wireless deployments, Access Points (APs) are often unmanaged, leading to interference that degrades performance. This project explores the **PARF (Power-Aware Rate Adaptation)** algorithm, designed to minimize interference while maintaining robust connectivity for mobile clients.

I implemented a modified version of the PARF algorithm in **ns-3**, introducing a dynamic `PowerStepSize` attribute to evaluate the trade-off between network throughput and energy consumption (transmit power).

## Technical Stack
* **Network Simulator:** ns-3 (C++)
* **Data Analysis:** Python (Pandas, Matplotlib)
* **Concepts:** 802.11a Standard, Power Control, Rate Adaptation, Signal-to-Noise Ratio (SNR), Packet Loss Rate.

## Key Features & Simulations
* **Algorithm Modification:** Enhanced the PARF manager to support variable power step increments.
* **Mobility Scenarios:** Simulated a Mobile Station (MS) moving away from an AP at different velocities (5 m/s to 20 m/s).
* **Performance Metrics:** * **Throughput:** Measuring data transfer efficiency.
    * **Transmit Power:** Analyzing the energy cost of maintaining the link.
    * **Packet Loss:** Tracking reliability under high-speed mobility.
* **Data Pipeline:** Exported simulation traces to CSV and processed them via a Python Notebook for automated visualization.

## Results Summary
The analysis revealed that while larger step sizes improve throughput and reduce packet loss in high-mobility scenarios, they significantly increase the energy footprint. This project highlights the critical balance required for energy-efficient wireless communication.

## Repository Structure
* `parf-wifi-manager.cc`: Core C++ implementation of the modified PARF algorithm.
* `wifi_template.cc`: Simulation script defining the network topology and mobility models.
* `PARF_wireless.ipynb`: Python notebook used for plotting results and statistical analysis.
* `PARF_wireless.pdf`: Full technical report with methodology, results, and conclusions.

---
*Project developed during the specialization in Mobile Wireless Networks (UC Riverside).*
