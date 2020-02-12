# source('https://raw.githubusercontent.com/gkantsidis/Utils/master/PL/R/Requirements.R')
source('https://raw.githubusercontent.com/gkantsidis/Utils/master/PL/R/Plots/Plotting.R')
source('https://raw.githubusercontent.com/gkantsidis/Utils/master/PL/R/Utils.R')
library(ggplot2)
library(ggrepel)
library(scales)

bit_errors <- read.csv(file.path(ez.csp(), 'inserted_bit_errors.csv'))

p <- make_plot(data=bit_errors) + aes(x = BitErrors, y = ExtraBits)
p <- p + geom_jitter(aes(col=Success))
p