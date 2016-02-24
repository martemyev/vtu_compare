#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void get_data(const std::string &filename, double *data, int N);
void write_difference(const std::string &filename,
                      const std::string &diff_file,
                      const std::vector<std::vector<double> > &diff);




int main(int argc, char **argv)
{
  if (argc < 5)
  {
    std::cout << "\nUsage: ./prog file_0.vtu file_1.vtu diff_file.vtu K1 K2 ... KN\n\n";
    std::cout << "file_0      - name of one file for comparison\n";
    std::cout << "file_1      - name of another file for comparison\n";
    std::cout << "diff_file   - name of file with difference\n";
    std::cout << "K1          - size of first distinct solution (total number of dofs)\n";
    std::cout << "K2          - size of second distinct solution (total number of dofs)\n";
    std::cout << "KN          - size of N-th distinct solution (total number of dofs)\n\n";
    return 1;
  }

  const std::string file_0    = std::string(argv[1]);
  const std::string file_1    = std::string(argv[2]);
  const std::string diff_file = std::string(argv[3]);

  std::vector<int> sizes(argc - 4);
  for (int i = 4; i < argc; ++i)
    sizes[i-4] = atoi(argv[i]);

  std::cout << "file_0    = " << file_0 << "\n";
  std::cout << "file_1    = " << file_1 << "\n";
  std::cout << "diff_file = " << diff_file << "\n";
  for (size_t i = 0; i < sizes.size(); ++i)
    std::cout << "K" << i+1 << "        = " << sizes[i] << "\n";


  double *data_0 = new double[N];
  double *data_1 = new double[N];
  double *diff   = new double[N];

  get_data(file_0, data_0, N);
  get_data(file_1, data_1, N);

  double L2_0 = 0, L2_1 = 0, L2_diff = 0;
  for (int i = 0; i < N; ++i)
  {
    const double d0 = data_0[i];
    const double d1 = data_1[i];
    L2_0 += d0 * d0;
    L2_1 += d1 * d1;
    diff[i] = d0 - d1;
    L2_diff += diff[i] * diff[i];
  }
//  for (int i = 0; i < 101; ++i)
//  {
//    for (int j = 0; j < 101*3; ++j)
//    {
//      const int k = i*101*3 + j;
//      const double d0 = data_0[(100-i)*101*3 + j];
//      const double d1 = data_1[k];
//      L2_0 += d0 * d0;
//      L2_1 += d1 * d1;
//      diff[k] = d0 - d1;
//      L2_diff += diff[k] * diff[k];
//    }
//  }

  L2_0 = sqrt(L2_0);
  L2_1 = sqrt(L2_1);
  L2_diff = sqrt(L2_diff);

  std::cout << "L2_0 = " << L2_0 << std::endl;
  std::cout << "L2_1 = " << L2_1 << std::endl;
  std::cout << "L2_diff = " << L2_diff << " = " << L2_diff * 100 << " %" << std::endl;

  if (!diff_file.empty())
    write_difference(file_0, diff_file, diff, N);

  delete[] data_0;
  delete[] data_1;
  delete[] diff;

  return 0;

}





void get_data(const std::string &filename, double *data, int N)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    std::cerr << "File " << filename << " can't be opened" << std::endl;
    throw 1;
  }

  std::string tmp; // temporary lines

  for (int i = 0; i < 6; ++i)
    getline(in, tmp);

  for (int i = 0; i < N; ++i)
    in >> data[i];

  in.close();
}




void write_difference(const std::string &filename,
                      const std::string &diff_file,
                      double *diff,
                      int N)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    std::cerr << "File " << filename << " can't be opened for reading\n";
    exit(1);
  }

  std::ofstream out(diff_file.c_str());
  if (!out)
  {
    {
      std::cerr << "File " << diff_file << " can't be opened for writing\n";
      exit(1);
    }
  }

  std::string line;

  for (int i = 0; i < 6; ++i)
  {
    getline(in, line);
    out << line << "\n";
  }

  double tmp;
  for (int i = 0; i < N; ++i)
  {
    in >> tmp;
    out << diff[i] << " ";
  }

  while (getline(in, line))
    out << line << "\n";

  out.close();
  in.close();
}

