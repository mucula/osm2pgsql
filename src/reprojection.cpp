#include <osmium/geom/mercator_projection.hpp>

#include "reprojection.hpp"

namespace {

Coordinates lonlat2merc(Coordinates coords)
{
    if (coords.y > 89.99) {
        coords.y = 89.99;
    } else if (coords.y < -89.99) {
        coords.y = -89.99;
    }

    return osmium::geom::lonlat_to_mercator(coords);
}

class latlon_reprojection_t : public reprojection
{
public:
    Coordinates reproject(osmium::Location loc) const noexcept override
    {
        return Coordinates{loc.lon_without_check(), loc.lat_without_check()};
    }

    Coordinates target_to_tile(Coordinates c) const noexcept override
    {
        return lonlat2merc(c);
    }

    int target_srs() const noexcept override { return PROJ_LATLONG; }

    char const *target_desc() const noexcept override { return "Latlong"; }
};

class merc_reprojection_t : public reprojection
{
public:
    Coordinates reproject(osmium::Location loc) const override
    {
        Coordinates const coords{loc.lon_without_check(),
                                 loc.lat_without_check()};
        return lonlat2merc(coords);
    }

    Coordinates target_to_tile(Coordinates c) const noexcept override
    {
        return c;
    }

    int target_srs() const noexcept override { return PROJ_SPHERE_MERC; }

    char const *target_desc() const noexcept override
    {
        return "Spherical Mercator";
    }
};

} // anonymous namespace

std::shared_ptr<reprojection> reprojection::create_projection(int srs)
{
    switch (srs) {
    case PROJ_LATLONG:
        return std::make_shared<latlon_reprojection_t>();
    case PROJ_SPHERE_MERC:
        return std::make_shared<merc_reprojection_t>();
    default:
        break;
    }

    return make_generic_projection(srs);
}
