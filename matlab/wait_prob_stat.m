close all;
more off;
fprintf('\n');

if (0)
clear all;

IN_DEPOT         = 1;
CHARGING         = 2;
SWITCHING        = 3;
REWARD           = 4;
FLAGS            = 5;
ENERGY           = 6;
T1_ROBOTS        = 7;
T1_OPT_WORKER    = 8;
T1_RTT           = 9;
T1_PROD_RATE     = 10;
T1_REWARD        = 11;
T1_FLAGS         = 12;
T2_ROBOTS        = 13;
T2_OPT_WORKER    = 14;
T2_RTT           = 15;
T2_PROD_RATE     = 16;
T2_REWARD        = 17;
T2_FLAGS         = 18;

dir = '../data';
prob = 0.0:0.1:1.0;

for i = 1:size(prob)(2)
  for j = 1:10
    filename = sprintf('%s/wait_prob_analyst_%0.1f_%d.log',dir, prob(i),j);
    data = load(filename);
    [N M] = size(data);
    T = 0.1 * N;
    avgInDepot(i,j) = mean(data(:, IN_DEPOT));
    avgT1Robots(i,j) = mean(data(:, T1_ROBOTS));
    avgT2Robots(i,j) = mean(data(:, T2_ROBOTS));
    optT1Robots(i,j) = data(N, T1_OPT_WORKER);
    optT2Robots(i,j) = data(N, T2_OPT_WORKER);
    wait_dr(i,j) = data(N, FLAGS) / T;
    wait_er(i,j) = data(N, ENERGY) / T;
    switching(i,j) = data(N, SWITCHING);
    flagsPreEnergy(i,j) = data(N, FLAGS) / data(N, ENERGY);
    fprintf('wait prob %0.1f: %f\n', prob(i), wait_dr(i,j));
  end % for j

  avgWaitDr(i) = mean(wait_dr(i,:));
  stdWaitDr(i) = std(wait_dr(i,:));
  avgWaitEr(i) = mean(wait_er(i,:));
  stdWaitEr(i) = std(wait_er(i,:));
  avgFlagPerEnergy(i) = avgWaitDr(i) / avgWaitEr(i);
  avgSwitching(i) = mean(switching(i,:));
  stdSwitching(i) = std(switching(i,:));
end % for i
end % if (load file)

figure;
errorbar( prob, avgWaitDr, stdWaitDr);
grid on;
title('Wait prob - broadcast probability analysis');
xlabel('Broadcast probability');
ylabel('Delivery rate');
legend("location", "northwest");
print('./images/wait_prob_dr.png', '-dpng');

figure;
errorbar( prob, avgWaitEr, stdWaitEr);
grid on;
title('Wait prob - broadcast probability analysis');
xlabel('Broadcast probability');
ylabel('Energy rate');
legend("location", "northwest");
print('./images/wait_prob_er.png', '-dpng');

figure;
plot(prob, avgFlagPerEnergy);
grid on;
xlabel('Broadcast probability');
ylabel('Flags/Energy');
legend("location", "northwest");

figure;
errorbar( prob, avgSwitching, stdSwitching);
grid on;
title('Task Switching');
xlabel('Broadcast probability');
ylabel('# task switch');
legend("location", "northwest");