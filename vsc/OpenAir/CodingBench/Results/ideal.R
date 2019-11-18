#' ---
#' title: "Performance of ideal binary channel"
#' author: "Christos Gkantsidis"
#' date: "2019-11-12"
#' ---

#' This script examines the performance of an ideal binary channel
#' with gaussian noise.

#+ setup, include=FALSE
source('https://raw.githubusercontent.com/gkantsidis/Utils/master/PL/R/Plots/Plotting.R')
source('https://raw.githubusercontent.com/gkantsidis/Utils/master/PL/R/Utils.R')
library(ggplot2)
library(ggrepel)
library(scales)

ideal <- read.csv(file.path(ez.csp(), 'ideal.csv'))
ideal$Overhead <- 100.0 * (1.0 - ideal$Capacity)
ideal$UncodedBerP <- 100.0 * ideal$UncodedBer

p <- make_plot(data=ideal) + aes(x = UncodedBerP, y = Overhead)
p <- p + xlab("Uncoded BER (%)") + ylab("Storage overhead (%)")
p <- p + geom_point(size=3.0)
p <- p + expand_limits(x = 0, y = 0)
p <- p + scale_x_continuous(expand = c(0, 0)) + scale_y_continuous(expand = c(0, 0))
p

s <- make_plot(data=ideal) + aes(x = UncodedBerP, y = ShannonCapacity)
s <- s + xlab("Uncoded BER (%)") + ylab("Channel Capacity (bits / symbol)")
s <- s + geom_point(size=3.0)
s <- s + expand_limits(x = 0, y = 0)
s <- s + scale_x_continuous(expand = c(0, 0)) + scale_y_continuous(expand = c(0, 0))
s


snr_for_rate <- read.csv(file.path(ez.csp(), 'snr_for_rate_binary_awgn.csv'))
s4r <- make_plot(data=snr_for_rate) + aes(x = Rate, y = SNR)
s4r <- s4r + geom_point(size=3.0)
s4r
