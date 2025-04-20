#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <complex>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std;
using namespace cv;

namespace FractalConfig {
    constexpr int IMAGE_WIDTH = 800;
    constexpr int IMAGE_HEIGHT = 800;
    constexpr int MAX_ITERATIONS = 1000;

    constexpr double X_MIN = -2.5;
    constexpr double X_MAX = 1.0;
    constexpr double Y_MIN = -1.5;
    constexpr double Y_MAX = 1.5;
}


int compute_mandelbrot(double real, double imag) {
    complex<double> z(0.0, 0.0);
    complex<double> c(real, imag);
    int iterations = 0;

    while (abs(z) < 2.0 && iterations < FractalConfig::MAX_ITERATIONS) {
        z = z * z + c;
        ++iterations;
    }

    return iterations;
}

Vec3b map_to_color(int iterations) {
    if (iterations >= FractalConfig::MAX_ITERATIONS) {
        return Vec3b(0, 0, 0);
    }

    float t = static_cast<float>(iterations) / FractalConfig::MAX_ITERATIONS;
    auto r = static_cast<unsigned char>(255.0 * pow(t, 0.5));
    auto g = static_cast<unsigned char>(255.0 * pow(t, 0.7));
    auto b = static_cast<unsigned char>(255.0 * pow(t, 0.9));

    return Vec3b(b, g, r);
}

void compute_fractal_region(Mat& image, int start_row, int row_count) {
    for (int y = 0; y < row_count; ++y) {
        for (int x = 0; x < FractalConfig::IMAGE_WIDTH; ++x) {
            double real = FractalConfig::X_MIN + x * (FractalConfig::X_MAX - FractalConfig::X_MIN) / FractalConfig::IMAGE_WIDTH;
            double imag = FractalConfig::Y_MIN + (start_row + y) * (FractalConfig::Y_MAX - FractalConfig::Y_MIN) / FractalConfig::IMAGE_HEIGHT;

            int iterations = compute_mandelbrot(real, imag);
            image.at<Vec3b>(y, x) = map_to_color(iterations);
        }
    }
}

double measure_time(const chrono::high_resolution_clock::time_point& start, const string& label) {
    auto end = chrono::high_resolution_clock::now();
    double duration = chrono::duration<double>(end - start).count();
    cout << label << ": " << duration << " seconds\n";
    return duration;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (rank == 0) {
        auto start = chrono::high_resolution_clock::now();
        Mat sequential_image(FractalConfig::IMAGE_HEIGHT, FractalConfig::IMAGE_WIDTH, CV_8UC3);
        compute_fractal_region(sequential_image, 0, FractalConfig::IMAGE_HEIGHT);
        measure_time(start, "Sequential execution time");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int rows_per_process = FractalConfig::IMAGE_HEIGHT / num_processes;
    int remaining_rows = FractalConfig::IMAGE_HEIGHT % num_processes;

    int start_row = rows_per_process * rank + min(rank, remaining_rows);
    int row_count = rows_per_process + (rank < remaining_rows ? 1 : 0);

    Mat local_image(row_count, FractalConfig::IMAGE_WIDTH, CV_8UC3);
    auto start_parallel = chrono::high_resolution_clock::now();
    compute_fractal_region(local_image, start_row, row_count);

    if (rank == 0) {
        Mat final_image(FractalConfig::IMAGE_HEIGHT, FractalConfig::IMAGE_WIDTH, CV_8UC3);
        local_image.copyTo(final_image(Rect(0, start_row, FractalConfig::IMAGE_WIDTH, row_count)));

        for (int proc = 1; proc < num_processes; ++proc) {
            int proc_start_row = rows_per_process * proc + min(proc, remaining_rows);
            int proc_row_count = rows_per_process + (proc < remaining_rows ? 1 : 0);

            vector<unsigned char> buffer(proc_row_count * FractalConfig::IMAGE_WIDTH * 3);
            MPI_Recv(buffer.data(), buffer.size(), MPI_UNSIGNED_CHAR, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Mat temp(proc_row_count, FractalConfig::IMAGE_WIDTH, CV_8UC3, buffer.data());
            temp.copyTo(final_image(Rect(0, proc_start_row, FractalConfig::IMAGE_WIDTH, proc_row_count)));
        }

        measure_time(start_parallel, "Parallel (MPI) execution time");

        imwrite("mandelbrot_parallel.png", final_image);
        imshow("Mandelbrot Fractal", final_image);
        waitKey(0);
    } else {
        int data_size = row_count * FractalConfig::IMAGE_WIDTH * 3;
        MPI_Send(local_image.data, data_size, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
