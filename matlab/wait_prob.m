close all;
more off;
fprintf('\n');
if (1)
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
	filename = sprintf('%s/wait_prob_analyst_%0.1f.log',dir, prob(i));
	data = load(filename);
	[N M] = size(data);
	T = 0.1 * N;
	avgInDepot(i) = mean(data(:, IN_DEPOT));
	avgT1Robots(i) = mean(data(:, T1_ROBOTS));
	avgT2Robots(i) = mean(data(:, T2_ROBOTS));
	optT1Robots(i) = data(N, T1_OPT_WORKER);
	optT2Robots(i) = data(N, T2_OPT_WORKER);
	wait_dr(i) = data(N, FLAGS) / T;
	wait_er(i) = data(N, ENERGY) / T;
	flagsPreEnergy(i) = data(N, FLAGS) / data(N, ENERGY);
   	fprintf('wait prob %f: %f\n', prob(i), wait_dr(i));
end

end % if (load file)

figure;
plot( prob, wait_dr, 'b*;delivery rate;');
grid on;
title('Wait prob - broadcast probability analysis');
xlabel('Broadcast probability');
ylabel('Delivery rate');
legend("location", "northwest");
print('./images/wait_prob_dr.png', '-dpng');

figure;
plot( prob, wait_er, 'g*;energy rate;');
grid on;
title('Wait prob - broadcast probability analysis');
xlabel('Broadcast probability');
ylabel('Energy rate');
legend("location", "northwest");
print('./images/wait_prob_er.png', '-dpng');

figure;
plot( prob, flagsPreEnergy, 'r*;flag/energy;');
grid on;
title('Wait prob - broadcast probability analysis');
xlabel('Broadcast probability');
ylabel('Flag/Energy');
print('./images/wait_prob_fe.png', '-dpng');

figure;
plot(avgInDepot, 'r-;in depot;');
hold on;
plot(avgT1Robots, 'g-;T1;');
plot(optT1Robots, 'g--;T1 opt;');
plot(avgT2Robots, 'b-;T2;');
plot(optT2Robots, 'b--;T2 opt;');
hold off;
grid on;
xlabel('Broadcast probability');
ylabel('#robots');