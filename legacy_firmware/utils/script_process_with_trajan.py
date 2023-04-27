from pathlib import Path
from trajan.readers.omb import read_omb_csv
import trajan as ta
import coloredlogs

# adjust the level of information printed
coloredlogs.install(level='error')
# coloredlogs.install(level='debug')

path_to_test_data = Path.cwd() / "all.csv"

# for most users, using the default spectra packet format and this will be enough
xr_result = read_omb_csv(path_to_test_data)

# remember to add the CF required metadata for your specific deployment
xr_result = xr_result.assign_attrs(
    {
        "creator_name": "your name",
        "creator_email": "your email",
        "title": "a descriptive title",
        "summary": "a descriptive summary",
        "anything_else": "corresponding data",
    }
)

# look at the dataset obtained
print(xr_result)

# dump to netCDF-CF file
path_to_netCDF_dump = Path.cwd() / "all.nc"
xr_result.to_netcdf(path_to_netCDF_dump)

