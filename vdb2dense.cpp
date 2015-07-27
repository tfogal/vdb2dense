#include <cstdlib>
#include <fstream>
#include <openvdb/openvdb.h>
#include <openvdb/tools/Dense.h>
#include <openvdb/tree/Tree.h>

// VDB file to read from
static char* inputfn = NULL;
// raw file to create.
static char* outputfn = NULL;

static void
help(const char* program) {
	printf("%s -i infile.vdb -o outfile [-h]\n", program);
}

static void
args(int argc, char* argv[]) {
	int o;
	while((o = getopt(argc, argv, "i:o:h")) != -1) {
		switch(o) {
		case 'i':
			inputfn = strdup(optarg);
			break;
		case 'o':
			outputfn = strdup(optarg);
			break;
		case 'h':
			help(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		}
	}
	if(inputfn == NULL) {
		fprintf(stderr, "Need to know what to read from!\n");
		help(argv[0]);
		exit(EXIT_FAILURE);
	}
	if(outputfn == NULL) {
		fprintf(stderr, "Need to know what to create!\n");
		help(argv[0]);
		exit(EXIT_FAILURE);
	}
}

struct ovdbinit {
	ovdbinit() { openvdb::v2_3::initialize(); }
	~ovdbinit() { openvdb::v2_3::uninitialize(); }
};

typedef openvdb::tools::Dense<float, openvdb::tools::LayoutZYX> DenseT;
static void
write_raw(std::string to, const DenseT& dtree) {
	using namespace openvdb;

	std::ofstream raw(to, std::ios::out | std::ios::binary);
	if(!raw.is_open()) {
		std::cerr << "Could not create " << to << "\n";
		return;
	}

	// grid is not at 0,0,0: offset by this amount.
	const size_t offset[3] = { 64, 39, 41 };
	const size_t dims[3] = { 264-offset[0], 310-offset[1], 270-offset[2] };

	const float* data = dtree.data();
	std::cout << dtree.valueCount() << " voxels.  Should be "
	          << dims[0]*dims[1]*dims[2] << "\n";
	std::cout << "bb:" << dtree.bbox().min() << "; " << dtree.bbox().max()
	          << "\n";
	for(size_t z=0; z < dims[2]; ++z) {
		for(size_t y=0; y < dims[1]; ++y) {
			for(size_t x=0; x < dims[0]; ++x) {
				const Coord c(x+offset[0],y+offset[1],z+offset[2]);
#if 0
				std::cout << "c " << c << "\n";
				std::cout << "x=" << x << ", dims[0]=" << dims[0] << "\n";
#endif
				const size_t off = dtree.coordToOffset(c);
				float f = data[off];
				raw.write(reinterpret_cast<char*>(&f), sizeof(float));
			}
		}
	}
	raw.close();
}

int main(int argc, char* argv[]) {
	args(argc, argv);
	using namespace openvdb;
	ovdbinit nonsense_variable_name_;

	openvdb::io::File vdb(inputfn);
	vdb.open();
	FloatGrid::Ptr grid = gridPtrCast<FloatGrid>(vdb.readGrid("density"));
	vdb.close();

	DenseT dense(grid->evalActiveVoxelBoundingBox());
	tools::copyToDense(*grid, dense);
	write_raw(outputfn, dense);
	return EXIT_SUCCESS;
}
